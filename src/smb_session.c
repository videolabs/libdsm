// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "bdsm/smb_session.h"

smb_session_t   *smb_session_new()
{
  smb_session_t *s;

  s = malloc(sizeof(smb_session_t));
  assert(s != NULL);
  memset((void *)s, 0, sizeof(smb_session_t));

  return (s);
}

void            smb_session_destroy(smb_session_t *s)
{
  if (s != NULL)
  {
    if (s->nb_session != NULL)
      netbios_session_destroy(s->nb_session);
    free(s);
  }
}

int             smb_session_connect(smb_session_t *s, char *name, uint32_t ip)
{
  assert(s != NULL && name != NULL);

  if ((s->nb_session = netbios_session_new(ip)) == NULL)
    goto error;
  if (!netbios_session_connect(s->nb_session, name))
    goto error;

  s->state = SMB_STATE_NETBIOS_OK;
  return (1);

  error:
    s->state = SMB_STATE_ERROR;
    return (0);
}

int             smb_session_send_msg(smb_session_t *s, smb_message_t *msg)
{
  size_t        packet_size;

  assert(s != NULL && s->state >= SMB_STATE_NETBIOS_OK && s->nb_session != NULL);
  assert(msg != NULL && msg->packet != NULL);

  netbios_session_packet_init(s->nb_session, NETBIOS_OP_SESSION_MSG);

  packet_size   = sizeof(netbios_session_packet_t) + sizeof(smb_packet_t);
  packet_size  += msg->cursor;
  if (!netbios_session_packet_append(s->nb_session, (char *)msg->packet, packet_size))
    return (0);
  if (!netbios_session_packet_send(s->nb_session))
    return (0);

  return (1);
}

size_t          smb_session_recv_msg(smb_session_t *s, smb_message_t *msg)
{
  netbios_session_packet_t  *nb_packet;
  ssize_t                   recv_size;
  size_t                    payload_size;

  assert(s != NULL && s->nb_session != NULL && msg != NULL);

  recv_size = netbios_session_packet_recv(s->nb_session);
  if(recv_size <= 0)
    return (0);

  nb_packet   = (netbios_session_packet_t *)s->nb_session->recv_buffer;
  msg->packet = (smb_packet_t *)nb_packet->payload;

  payload_size  = ntohs(nb_packet->length);
  payload_size |= (nb_packet->flags & 0x01) << 16; // XXX If this is the case we overran our recv_buffer
  if (payload_size > recv_size - sizeof(netbios_session_packet_t))
  {
    if (BDSM_DEBUG)
      fprintf(stderr, "smb_session_recv_msg: Packet size mismatch\n");
    return(0);
  }

  msg->payload_size = payload_size - sizeof(smb_header_t);
  msg->cursor       = 0;

  return(msg->payload_size);
}

int             smb_session_negotiate_protocol(smb_session_t *s)
{
  const char            *dialects[] = SMB_DIALECTS;
  smb_message_t         *msg = NULL;
  smb_message_t         answer;
  smb_negotiate_resp_t  *nego;


  msg = smb_message_new(SMB_CMD_NEGOTIATE, 64);
  smb_message_set_default_flags(msg);

  smb_message_put8(msg, 0);   // wct
  smb_message_put16(msg, 0);  // bct, will be updated later

  for(unsigned i = 0; dialects[i] != NULL; i++)
    smb_message_append(msg, dialects[i], strlen(dialects[i]) + 1);
  *((uint16_t *)(msg->packet->payload + 1)) = msg->cursor - 3;

  if (!smb_session_send_msg(s, msg))
  {
    smb_message_destroy(msg);
    goto error;
  }
  smb_message_destroy(msg);

  if (!smb_session_recv_msg(s, &answer))
    goto error;

  nego = (smb_negotiate_resp_t *)answer.packet->payload;
  if (answer.packet->header.status != NT_STATUS_SUCCESS
      && nego->wct != 0x11 && nego->security_mode & 0x03)
    goto error;

  s->srv.dialect        = nego->dialect_index;
  s->srv.security_mode  = nego->security_mode;
  s->srv.caps           = nego->caps;
  s->srv.session_key    = nego->session_key;
  s->srv.challenge      = nego->challenge;

  // Yeah !
  s->state              = SMB_STATE_DIALECT_OK;

  return (1);

  error:
    s->state = SMB_STATE_ERROR;
    return (0);
}
