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
#include <libtasn1.h>
#include <stdbool.h>

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

// An structure to store user credentials;
// login:password@domain (also DOMAIN\login)
typedef struct
{
    char     *domain;
    char     *login;
    char     *password;
}           smb_creds;

/**
 * @brief An opaque data structure to represent a SMB Session.
 */
typedef struct smb_session smb_session;

/**
 * @struct smb_share_list
 * @brief An opaque object representing the list of share of a SMB file server.
 */
typedef char  **smb_share_list;

/**
 * @brief An opaque data structure to represent file
 */
typedef struct smb_file smb_file;

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

#endif
