// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_SMB_SESSION_H_
#define __BDSM_SMB_SESSION_H_

#include "bdsm/netbios_session.h"
#include "bdsm/smb_defs.h"
#include "bdsm/smb_message.h"

#define SMB_STATE_DIALECT_OK    2
#define SMB_STATE_NETBIOS_OK    1
#define SMB_STATE_NEW           0
#define SMB_STATE_ERROR         -1

typedef struct
{
  int                 state;
  netbios_session_t   *nb_session;
  struct {
    uint16_t            dialect;        // The selected dialect
    uint16_t            security_mode;  // Security mode
    uint32_t            caps;           // Server caps replyed during negotiate
    uint32_t            session_key;    // XXX Is this really usefull?
    uint64_t            challenge;      // For challenge response security
  }                   srv;
}                   smb_session_t;


smb_session_t   *smb_session_new();
void            smb_session_destroy(smb_session_t *s);

int             smb_session_send_msg(smb_session_t *s, smb_message_t *msg);
// msg->packet will be updated to point on received data. You don't own this
// memory. It'll be reused on next recv_msg
size_t          smb_session_recv_msg(smb_session_t *s, smb_message_t *msg);
int             smb_session_connect(smb_session_t *s, char *name, uint32_t ip);
int             smb_session_negotiate_protocol(smb_session_t *s);

#endif
