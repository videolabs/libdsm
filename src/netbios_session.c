//---------------------------------------------------------------------------
//  __________________    _________  _____            _____  .__         ._.
//  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
//   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
//   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
//   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
//          \/        \/        \/        \/  )/           \/        \/   \/
//
// This file is part of libdsm. Copyright © 2014 VideoLabs SAS
//
// Author: Julien 'Lta' BALLET <contact@lta.io>
//
// This program is free software. It comes without any warranty, to the extent
// permitted by applicable law. You can redistribute it and/or modify it under
// the terms of the Do What The Fuck You Want To Public License, Version 2, as
// published by Sam Hocevar. See the COPYING file for more details.
//----------------------------------------------------------------------------

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bdsm/debug.h"
#include "bdsm/netbios_session.h"
#include "bdsm/netbios_utils.h"

static int        open_socket_and_connect(netbios_session *s)
{
  if ((s->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    goto error;
  if (connect(s->socket, (struct sockaddr *)&s->remote_addr, sizeof(s->remote_addr)) <0)
    goto error;

  return (1);

  error:
    perror("netbios_session_new, open_socket: ");
    return (0);
}

netbios_session *netbios_session_new(size_t buf_size)
{
  netbios_session   *session;
  size_t            packet_size;

  session = (netbios_session *)malloc(sizeof(netbios_session));
  assert(session != NULL);
  memset((void *) session, 0, sizeof(netbios_session));

  session->packet_payload_size = buf_size;
  packet_size = sizeof(netbios_session_packet) + session->packet_payload_size;
  session->packet = (netbios_session_packet *)malloc(packet_size);
  assert(session->packet != NULL);

  return(session);
}

void              netbios_session_destroy(netbios_session *s)
{
  if (!s)
    return;
  close(s->socket);
  if (s->packet)
    free(s->packet);
  free(s);
}

int               netbios_session_connect(struct in_addr *addr,
                                          netbios_session *s,
                                          const char *name,
                                          int direct_tcp)
{
  ssize_t                   recv_size;
  char                      *encoded_name;

  assert(s != NULL && s->packet != NULL);

  if (direct_tcp)
    s->remote_addr.sin_port       = htons(NETBIOS_PORT_DIRECT);
  else
    s->remote_addr.sin_port       = htons(NETBIOS_PORT_SESSION);
  s->remote_addr.sin_family       = AF_INET;
  s->remote_addr.sin_addr.s_addr  = addr->s_addr;
  if (!open_socket_and_connect(s))
    goto error;

  if (!direct_tcp)
  {
  // Send the Session Request message
    netbios_session_packet_init(s);
    s->packet->opcode = NETBIOS_OP_SESSION_REQ;
    encoded_name = netbios_name_encode(name, 0, NETBIOS_FILESERVER);
    netbios_session_packet_append(s, encoded_name, strlen(encoded_name) + 1);
    free(encoded_name);
    encoded_name = netbios_name_encode("LIBDSM", 0, NETBIOS_WORKSTATION);
    netbios_session_packet_append(s, encoded_name, strlen(encoded_name) + 1);
    free(encoded_name);

    s->state = NETBIOS_SESSION_CONNECTING;
    if (!netbios_session_packet_send(s))
      goto error;

    // Now receiving the reply from the server.
    recv_size = netbios_session_packet_recv(s, NULL);
    if (recv_size < 0)
      goto error;

    // Reply was negative, we are not connected :(
    if (s->packet->opcode != NETBIOS_OP_SESSION_REQ_OK)
    {
      s->state = NETBIOS_SESSION_REFUSED;
      return (0);
    }
  }

  // Reply was OK or DirectTCP, a session has been established
  s->state = NETBIOS_SESSION_CONNECTED;
  return(1);

  error:
    s->state = NETBIOS_SESSION_ERROR;
    return (0);
}

void              netbios_session_packet_init(netbios_session *s)
{
  assert(s != NULL);

  s->packet_cursor = 0;
  s->packet->opcode = NETBIOS_OP_SESSION_MSG;
}

int               netbios_session_packet_append(netbios_session *s,
                                                const char *data, size_t size)
{
  char  *start;

  assert(s && s->packet);

  if (s->packet_payload_size - s->packet_cursor < size)
    return (0);

  start = ((char *)&s->packet->payload) + s->packet_cursor;
  memcpy(start, data, size);
  s->packet_cursor += size;

  return (1);
}

int               netbios_session_packet_send(netbios_session *s)
{
  ssize_t         to_send;
  ssize_t         sent;

  assert(s && s->packet && s->socket && s->state > 0);

  s->packet->length = htons(s->packet_cursor);
  to_send           = sizeof(netbios_session_packet) + s->packet_cursor;
  sent              = send(s->socket, (void *)s->packet, to_send, 0);

  if (sent != to_send)
  {
    perror("netbios_session_packet_send: Unable to send (full?) packet");
    return (0);
  }

  return (sent);
}

ssize_t           netbios_session_packet_recv(netbios_session *s, void **data)
{
  ssize_t         recv_size;
  size_t          len;

  assert(s != NULL && s->packet != NULL && s->socket && s->state > 0);

  recv_size = recv(s->socket, (void *)(s->packet), s->packet_payload_size, 0);

  if (recv_size < 0)
    perror("netbios_session_packet_recv: ");

  if (recv_size >= sizeof(netbios_session_packet) && data != NULL)
    *data = (void *)s->packet->payload;
  else if (data != NULL)
    *data = NULL;

  len  = ntohs(s->packet->length);
  len |= (s->packet->flags & 0x01) << 16;

  if (len != recv_size - sizeof(netbios_session_packet))
  {
    BDSM_dbg("netbios_session_packet_recv: Packet size mismatch\n");
    return(-1);
  }

  return (len);
}

