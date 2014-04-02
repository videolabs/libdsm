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

#ifndef __BDSM_SMB_SESSION_H_
#define __BDSM_SMB_SESSION_H_

#include "bdsm/netbios_session.h"
#include "bdsm/smb_defs.h"
#include "bdsm/smb_message.h"

#define SMB_STATE_SESSION_OK    3 // We are authenticated and can do stuff
#define SMB_STATE_DIALECT_OK    2
#define SMB_STATE_NETBIOS_OK    1
#define SMB_STATE_NEW           0
#define SMB_STATE_ERROR         -1

// The id of a connection to a share, i.e. 'TreeID'
typedef uint16_t    smb_tid;

// The id of a file handle, i.e. 'FUID'
typedef uint16_t    smb_fid;

// Concatenation of the two above, representing a file inside of a session
// First 4 bytes are the TreeID (smb_tid), last 4 are the File ID (FUID)
typedef uint32_t    smb_fd;

#define SMB_FD_TID(fd)    ((smb_tid)(fd >> 16))
#define SMB_FD_FID(fd)    ((smb_fid)(fd & 0x0000ffff))
#define SMB_FD(tid, fid)  ((((smb_fd)tid) << 16) | (((smb_fd) fid)))

typedef struct smb_file_s
{
  smb_fid             fid;
  struct smb_file_s   *next;          // Next file in this share
  smb_tid             tid;
  uint64_t            created;
  uint64_t            accessed;
  uint64_t            written;
  uint64_t            changed;
  uint64_t            alloc_size;
  uint64_t            size;
  uint32_t            attr;
} smb_file_t;

typedef struct smb_share_s
{
  smb_tid             tid;
  struct smb_share_s  *next;          // Next share in this session
  uint16_t            opts;           // Optionnal support opts
  uint16_t            rights;         // Maximum rights field
  uint16_t            guest_rights;
  smb_file_t          *files;         // List of all open files for this share
} smb_share_t;

typedef struct
{
  int                 state;
  int                 guest;            // boolean, are we logged as guest ?
  uint16_t            uid;              // uid attributed by the server
  netbios_session_t   *nb_session;

  // Informations about the smb server we are connected to.
  struct {
    uint16_t            dialect;        // The selected dialect
    uint16_t            security_mode;  // Security mode
    uint32_t            caps;           // Server caps replyed during negotiate
    uint32_t            session_key;    // XXX Is this really usefull?
    uint64_t            challenge;      // For challenge response security
    uint64_t            ts;             // It seems Win7 requires it :-/
  }                   srv;

  smb_share_t         *shares;          // shares->files | Map fd <-> smb_file_t
}                   smb_session_t;


smb_session_t   *smb_session_new();
void            smb_session_destroy(smb_session_t *s);

int             smb_session_send_msg(smb_session_t *s, smb_message_t *msg);
// msg->packet will be updated to point on received data. You don't own this
// memory. It'll be reused on next recv_msg
size_t          smb_session_recv_msg(smb_session_t *s, smb_message_t *msg);
int             smb_session_connect(smb_session_t *s, char *name, uint32_t ip);
int             smb_negotiate(smb_session_t *s);
int             smb_authenticate(smb_session_t *s, const char *domain,
                                 const char *user, const char *password);

#endif
