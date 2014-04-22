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

/**
 * @file smb_session.h
 * @brief Functions to connect and authenticate to an SMB server
 */

/// Session Authentication was successfull, you can become nasty
#define SMB_STATE_SESSION_OK    3
/// Dialect was successfully negotiated
#define SMB_STATE_DIALECT_OK    2
/// A Netbios session has been successfully established.
#define SMB_STATE_NETBIOS_OK    1
/// The SMB session has just been created
#define SMB_STATE_NEW           0
/// Error state, there was an error somewhere
#define SMB_STATE_ERROR         -1

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

/** @struct smb_fd
  * @brief SMB File descriptor, represents a file within a session.
  */
typedef uint32_t    smb_fd;

/**
 * @internal
 * @brief Get a smb_tid from a smb_fd
 *
 * @param fd a smb_fd
 * @return A smb_tid
 */
#define SMB_FD_TID(fd)    ((smb_tid)(fd >> 16))
/**
 * @internal
 * @brief Get a smb_fid from a smb_fd
 *
 * @param fd a smb_fid
 * @return A smb_fid
 */
#define SMB_FD_FID(fd)    ((smb_fid)(fd & 0x0000ffff))
/**
 * @internal
 * @brief Compute the smb_fd for the given smb_tid and smb_fid
 * @param tid a smb_tid
 * @param fid a smb_fid *
 * @return A smb_fd
 */
#define SMB_FD(tid, fid)  ((((smb_fd)tid) << 16) | (((smb_fd) fid)))

/**
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
} smb_file_t;

typedef struct smb_share_s
{
  struct smb_share_s  *next;          // Next share in this session
  struct smb_file_s   *files;         // List of all open files for this share
  smb_tid             tid;
  uint16_t            opts;           // Optionnal support opts
  uint16_t            rights;         // Maximum rights field
  uint16_t            guest_rights;
} smb_share_t;

/**
 * @brief An opaque data structure to represent a SMB Session.
 */
typedef struct
{
  int                 state;
  int                 guest;            // boolean, are we logged as guest ?
  uint16_t            uid;              // uid attributed by the server
  netbios_session_t   *nb_session;

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

  struct smb_share_s  *shares;          // shares->files | Map fd <-> smb_file_t
}                   smb_session_t;



/**
 * @brief Allocates a new Session object
 * @details To be able to perform actions on shares and file, you'll need to
 * call smb_session_connect, then authenticate with smb_authenticate.
 * @return A new Session object.
 */
smb_session_t   *smb_session_new();

/**
 * @brief Close a session and deallocate its ressources
 * @details After destroying a session, all the smb_tid, smb_fid and smb_fd
 * associated with this session becomes invalid.
 *
 * @param s The session to destroy
 */
void            smb_session_destroy(smb_session_t *s);


/**
 * @brief Returns the current state of the session
 * @see SMB_STATE_ERROR
 * @see SMB_STATE_NEW
 * @see SMB_STATE_SESSION_OK
 */
int             smb_session_state(smb_session_t *s);


// --------------------------------
// Move me somewhere else !
void            smb_session_share_add(smb_session_t *s, smb_share_t *share);
smb_share_t     *smb_session_share_get(smb_session_t *s, smb_tid tid);
smb_share_t     *smb_session_share_remove(smb_session_t *s, smb_tid tid);

int             smb_session_file_add(smb_session_t *s, smb_tid tid, smb_file_t *f);
smb_file_t      *smb_session_file_get(smb_session_t *s, smb_fd fd);
smb_file_t      *smb_session_file_remove(smb_session_t *s, smb_fd fd);

int             smb_negotiate(smb_session_t *s);
int             smb_session_send_msg(smb_session_t *s, smb_message_t *msg);
// msg->packet will be updated to point on received data. You don't own this
// memory. It'll be reused on next recv_msg
size_t          smb_session_recv_msg(smb_session_t *s, smb_message_t *msg);
// --------------------------------

/**
 * @brief Establish a connection and negotiate a session protocol with a remote
 * host
 * @details You have to provide both the ip and the name. This is a constraint
 * of Netbios, which requires you to know its name before he accepts to speak
 * with you.
 *
 * @param s A session object.
 * @param name The ASCII netbios name, the name type will be coerced to <20>
 * @param ip The ip of the machine to connect to (in network byte order)
 * @return 0 in case of error, a value > 0 otherwise.
 */
int             smb_session_connect(smb_session_t *s, const char *name,
                                    uint32_t ip);

/**
 * @brief Authenticate on the remote host with the provided credentials
 * @details Can be called if session state is SMB_STATE_DIALECT_OK.
 * If successfull, session state transition to SMB_STATE_SESSION_OK
 * @bug We currently only support LMv2 authentication.
 *
 * @param s The session object.
 * @param domain The domain of the user. You should use the (netbios) uppercased
 * machine name, then try 'WORKGROUP'.
 * @param user The user name in the current locale
 * @param password The password in the current locale
 * @return 0 in case of failure, > 0 in case of success. Success doesn't mean
 * you are logged in with the user you requested. If guest are activated on
 * the remote host, when login fails, you are logged in as 'Guest'.
 */
int             smb_authenticate(smb_session_t *s, const char *domain,
                                 const char *user, const char *password);

#endif
