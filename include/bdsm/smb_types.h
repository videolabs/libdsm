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

/**
 * @file smb_types.h
 * @brief liBDSM types and structures
 */

#ifndef __BDSM_SMB_TYPES_H_
#define __BDSM_SMB_TYPES_H_

#include <netinet/ip.h>
#include <stddef.h>

#include <gssapi/gssapi_spnego.h>
#include <gssapi/gssapi_ntlm.h>

#include "bdsm/smb_packets.h"

/**
  * @struct smb_tid
  * @brief The id of a connection to a share within a session.
  */
typedef uint16_t    smb_tid;

/**
  * @struct smb_fid
  * @brief The id of a file within a share within a session.
  */
typedef uint16_t    smb_fid;

// Concatenation of the two above, representing a file inside of a session
// First 4 bytes are the TreeID (smb_tid), last 4 are the File ID (FUID)
// A map between smb_fd and smb_file is maintained inside each session
/** @struct smb_fd
  * @brief SMB File descriptor, represents a file within a session.
  */
typedef uint32_t    smb_fd;

/**
 * @internal
 * @struct smb_file
 * @brief An opaque data structure to represent file
 */
typedef struct  smb_file_s
{
  struct smb_file_s   *next;          // Next file in this share
  char                *name;
  smb_fid             fid;
  smb_tid             tid;
  size_t              name_len;
  uint64_t            created;
  uint64_t            accessed;
  uint64_t            written;
  uint64_t            changed;
  uint64_t            alloc_size;
  uint64_t            size;
  uint32_t            attr;
  uint32_t            readp;          // Current read pointer (position);
  int                 is_dir;         // 0 -> file, 1 -> directory
} smb_file;

typedef struct smb_share_s
{
  struct smb_share_s  *next;          // Next share in this session
  struct smb_file_s   *files;         // List of all open files for this share
  smb_tid             tid;
  uint16_t            opts;           // Optionnal support opts
  uint16_t            rights;         // Maximum rights field
  uint16_t            guest_rights;
} smb_share;

typedef struct smb_transport_s {
  void              *session;
  void              *(*new)(size_t buf_size);
  int               (*connect)(struct in_addr *addr, void *s, const char *name);
  void              (*destroy)(void *s);
  void              (*pkt_init)(void *s);
  int               (*pkt_append)(void *s, void *data, size_t size);
  int               (*send)(void *s);
  ssize_t           (*recv)(void *s, void **data);
}                   smb_transport;

/**
 * @brief An opaque data structure to represent a SMB Session.
 */
typedef struct
{
  int                 state;
  int                 guest;            // boolean, are we logged as guest ?
  uint16_t            uid;              // uid attributed by the server

  // Informations about the smb server we are connected to.
  struct {
    char                name[16];       // The server name
    uint16_t            dialect;        // The selected dialect
    uint16_t            security_mode;  // Security mode
    uint32_t            caps;           // Server caps replyed during negotiate
    uint32_t            session_key;    // XXX Is this really usefull?
    uint64_t            challenge;      // For challenge response security
    uint64_t            ts;             // It seems Win7 requires it :-/
  }                   srv;
  struct {
    gss_cred_id_t     credentials;
    gss_ctx_id_t      ctx;
    gss_buffer_desc   spnego;
  }                   gss;              // eXtended SECurity negociation data

  smb_transport       transport;
  struct smb_share_s  *shares;          // shares->files | Map fd <-> smb_file
}                   smb_session;

/**
 * @struct smb_share_list
 * @brief An opaque object representing the list of share of a SMB file server.
 */
typedef char  **smb_share_list;

/**
 * @struct smb_stat_list
 * @brief An opaque structure containing a list of file status
 */
typedef smb_file *smb_stat_list;

/**
 * @struct smb_stat
 * @brief An opaque structure containing info about a file
 */
typedef smb_file *smb_stat;

/**
 * @internal
 * @struct smb_message
 * @brief A convenience structure used to build smb messages
 */
typedef struct
{
  size_t          payload_size; // Size of the allocated payload
  size_t          cursor;       // Write cursor in the payload
  smb_packet      *packet;      // Yummy yummy, Fruity fruity !
}                               smb_message;


#endif
