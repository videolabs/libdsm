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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "bdsm/debug.h"
#include "bdsm/smb_session.h"
#include "bdsm/smb_session_msg.h"
#include "bdsm/smb_ntlm.h"
#include "bdsm/smb_spnego.h"
#include "bdsm/smb_transport.h"

static int        smb_negotiate(smb_session *s, int xsec);

smb_session   *smb_session_new()
{
  smb_session *s;

  s = malloc(sizeof(smb_session));
  assert(s != NULL);
  memset((void *)s, 0, sizeof(smb_session));

  s->guest              = false;

  // Explicitly sets pointer to NULL, insted of 0
  s->spnego_asn1        = NULL;
  s->transport.session  = NULL;
  s->shares             = NULL;

  s->creds.domain       = NULL;
  s->creds.login        = NULL;
  s->creds.password     = NULL;

  smb_buffer_init(&s->xsec_target, NULL, 0);

  // Until we know more, assume server supports everything.
  // s->c

  return (s);
}

void            smb_session_destroy(smb_session *s)
{
  if (s != NULL)
  {
    // FIXME Free smb_share and smb_file
    if (s->transport.session != NULL)
    {
      s->transport.destroy(s->transport.session);
      s->transport.session = NULL;
    }

    if (s->spnego_asn1 != NULL)
      asn1_delete_structure(&s->spnego_asn1);

    smb_buffer_free(&s->xsec_target);

    // Free stored credentials.
    free(s->creds.domain);
    free(s->creds.login);
    free(s->creds.password);
    free(s);
  }
}

int             smb_session_state(smb_session *s)
{
  if (s != NULL)
    return (s->state);
  else
    return (SMB_STATE_ERROR);
}

void            smb_session_set_creds(smb_session *s, const char *domain,
                                      const char *login, const char *password)
{
  assert(s != NULL);

  if (domain != NULL)
  {
    if (s->creds.domain != NULL)
      free(s->creds.domain);
    s->creds.domain = strndup(domain, SMB_CREDS_MAXLEN);
  }

  if (login != NULL)
  {
    if (s->creds.login != NULL)
      free(s->creds.login);
    s->creds.login = strndup(login, SMB_CREDS_MAXLEN);
  }

  if (password != NULL)
  {
    if (s->creds.password != NULL)
      free(s->creds.password);
    s->creds.password = strndup(password, SMB_CREDS_MAXLEN);
  }
}

int             smb_session_connect(smb_session *s, const char *name,
                                    uint32_t ip, int transport)
{
  assert(s != NULL && name != NULL);

  if (s->transport.session != NULL)
    s->transport.destroy(s->transport.session);

  switch (transport)
  {
    case SMB_TRANSPORT_TCP:
      smb_transport_tcp(&s->transport);
      break;
    case SMB_TRANSPORT_NBT:
      smb_transport_nbt(&s->transport);
      break;
    default:
      goto error;
  }

  if ((s->transport.session = s->transport.new(SMB_IO_BUFSIZE)) == NULL)
    goto error;
  if (!s->transport.connect((struct in_addr *)&ip, s->transport.session, name))
    goto error;

  memcpy(s->srv.name, name, strlen(name) + 1);
  s->state = SMB_STATE_NETBIOS_OK;

  if (!smb_negotiate(s, 1))     // Try to negotiate with extended security
    if (!smb_negotiate(s, 0))   // Try to negotiate withOUT extended security
      return (0);

  return(1);

  error:
    s->state = SMB_STATE_ERROR;
    return (0);
}

// xsec == 1 -> add Extended security flag
static int        smb_negotiate(smb_session *s, int xsec)
{
  (void)xsec; // FIXME

  const char          *dialects[] = SMB_DIALECTS;
  smb_message         *msg = NULL;
  smb_message         answer;
  smb_nego_resp       *nego;


  msg = smb_message_new(SMB_CMD_NEGOTIATE, 128);

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

  nego = (smb_nego_resp *)answer.packet->payload;
  if (answer.packet->header.status != NT_STATUS_SUCCESS && nego->wct != 0x11)
    goto error;

  s->srv.dialect        = nego->dialect_index;
  s->srv.security_mode  = nego->security_mode;
  s->srv.caps           = nego->caps;
  s->srv.ts             = nego->ts;
  s->srv.session_key    = nego->session_key;

  // Copy SPNEGO supported mechanisms  token for later usage (login_gss())
  if (smb_session_supports(s, SMB_SESSION_XSEC))
    BDSM_dbg("Server is supporting extended security\n");
  else
    s->srv.challenge      = nego->challenge;

  // Yeah !
  s->state              = SMB_STATE_DIALECT_OK;

  return (1);

  error:
    s->state = SMB_STATE_ERROR;
    return (0);
}


static int        smb_session_login_ntlm(smb_session *s, const char *domain,
                                         const char *user, const char *password)
{
  smb_message           answer;
  smb_message           *msg = NULL;
  smb_session_req       *req = NULL;
  uint8_t               *ntlm2 = NULL;
  smb_ntlmh             hash_v2;
  uint64_t              user_challenge;
  //uint8_t               blob[128];
  //size_t                blob_size;

  msg = smb_message_new(SMB_CMD_SETUP, 512);
  smb_message_set_andx_members(msg);

  req = (smb_session_req *)msg->packet->payload;
  req->wct              = 13;
  req->max_buffer       = SMB_SESSION_MAX_BUFFER;
  req->mpx_count        = 16; // XXX ?
  req->vc_count         = 1;
  //req->session_key      = s->srv.session_key; // XXX Useless on the wire?
  req->caps             = s->srv.caps; // XXX caps & our_caps_mask

  smb_message_advance(msg, sizeof(smb_session_req));

  user_challenge = smb_ntlm_generate_challenge();

  // LM2 Response
  smb_ntlm2_hash(user, password, domain, &hash_v2);
  ntlm2 = smb_lm2_response(&hash_v2, s->srv.challenge, user_challenge);
  smb_message_append(msg, ntlm2, 16 + 8);
  free(ntlm2);

  // NTLM2 Response (Win7 unsupported, replaced by extended security)
  // blob_size = smb_ntlm_make_blob((smb_ntlm_blob *)blob, s->srv.ts,
  //                                user_challenge, domain);
  // ntlm2 = smb_ntlm2_response(&hash_v2, s->srv.challenge, blob, blob_size);
  // smb_message_append(msg, ntlm2, 16 + blob_size);
  // free(ntlm2);

  req->oem_pass_len = 16 + SMB_LM2_BLOB_SIZE;
  req->uni_pass_len = 0; //16 + blob_size; //SMB_NTLM2_BLOB_SIZE;
  if (msg->cursor / 2) // Padding !
    smb_message_put8(msg, 0);

  smb_message_put_utf16(msg, user, strlen(user));
  smb_message_put16(msg, 0);
  smb_message_put_utf16(msg, domain, strlen(domain));
  smb_message_put16(msg, 0);
  smb_message_put_utf16(msg, SMB_OS, strlen(SMB_OS));
  smb_message_put16(msg, 0);
  smb_message_put_utf16(msg, SMB_LANMAN, strlen(SMB_LANMAN));
  smb_message_put16(msg, 0);

  req->payload_size = msg->cursor - sizeof(smb_session_req);

  if (!smb_session_send_msg(s, msg))
  {
    smb_message_destroy(msg);
    BDSM_dbg("Unable to send Session Setup AndX message\n");
    return (0);
  }
  smb_message_destroy(msg);

  if (smb_session_recv_msg(s, &answer) == 0)
  {
    BDSM_dbg("Unable to get Session Setup AndX reply\n");
    return (0);
  }

  smb_session_resp *r = (smb_session_resp *)answer.packet->payload;
  if (answer.packet->header.status != NT_STATUS_SUCCESS)
  {
    BDSM_dbg("Session Setup AndX : failure.\n");
    return (0);
  }

  if (r->action & 0x0001)
    s->guest = true;

  s->srv.uid  = answer.packet->header.uid;
  s->state    = SMB_STATE_SESSION_OK;

  return (1);
}

int             smb_session_login(smb_session *s)
{
  assert(s != NULL);

  if (smb_session_supports(s, SMB_SESSION_XSEC))
    return(smb_session_login_spnego(s, s->creds.domain, s->creds.login,
                                    s->creds.password));
  else
    return(smb_session_login_ntlm(s, s->creds.domain, s->creds.login,
                                  s->creds.password));
}


int             smb_session_is_guest(smb_session *s)
{
  // Invalid session object
  if (s == NULL)
    return (-1);

  // We're not logged in yet.
  if (smb_session_state(s) != SMB_STATE_SESSION_OK)
    return (-1);

  // We're logged in as guest
  if (s->guest)
    return (1);

  // We're logged in as regular user
  return (0);
}

const char      *smb_session_server_name(smb_session *s)
{
  if (s == NULL)
    return (NULL);
  else
    return (s->srv.name);
}

int             smb_session_supports(smb_session *s, int what)
{
  if (s == NULL)
    return (0);

  switch (what)
  {
    case SMB_SESSION_XSEC:
      return (s->srv.caps & SMB_CAPS_XSEC);
    default:
      return (0);
  }
}
