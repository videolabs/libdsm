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
#include "bdsm/smb_types.h"
#include "bdsm/smb_message.h"

/**
 * @file smb_session.h
 * @brief Functions to connect and authenticate to an SMB server
 */

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
 * @brief Allocates a new Session object
 * @details To be able to perform actions on shares and file, you'll need to
 * call smb_session_connect, then authenticate with smb_authenticate.
 * @return A new Session object.
 */
smb_session     *smb_session_new();

/**
 * @brief Close a session and deallocate its ressources
 * @details After destroying a session, all the smb_tid, smb_fid and smb_fd
 * associated with this session becomes invalid.
 *
 * @param s The session to destroy
 */
void            smb_session_destroy(smb_session *s);


/**
 * @brief Returns the current state of the session
 * @see SMB_STATE_ERROR
 * @see SMB_STATE_NEW
 * @see SMB_STATE_SESSION_OK
 */
int             smb_session_state(smb_session *s);



// ---------------------------------
// Internal stuff, maybe move this somewhere else
int             smb_session_send_msg(smb_session *s, smb_message *msg);
// msg->packet will be updated to point on received data. You don't own this
// memory. It'll be reused on next recv_msg
ssize_t         smb_session_recv_msg(smb_session *s, smb_message *msg);
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
 * since libdsm is about reading files
 * @param ip The ip of the machine to connect to (in network byte order)
 * @param transport The type of transport used, it could be SMB_TRANSPORT_TCP
 * or SMB_TRANSPORT_NBT (Netbios over TCP, ie legacy)
 * @return 0 in case of error, a value > 0 otherwise.
 */
int             smb_session_connect(smb_session *s, const char *name,
                                    uint32_t ip, int transport);

/**
 * @brief Authenticate on the remote host with the provided credentials
 * @details Can be called if session state is SMB_STATE_DIALECT_OK.
 * If successfull, session state transition to SMB_STATE_SESSION_OK
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
int             smb_session_login(smb_session *s, const char *domain,
                                  const char *user, const char *password);

/**
 * @brief Am i logged in as Guest ?
 *
 * @param s The session object
 * @return 1  -> Logged in as guest
 * 0  -> Logged in as regular user
 * -1 -> Error (not logged in, invalid session, etc.)
 */
int             smb_session_is_guest(smb_session *s);

/**
 * @brief Returns the server name with the <XX> type
 *
 * @param s The session object
 * @return The server name or NULL. The memory is owned by the session object.
 */
const char      *smb_session_server_name(smb_session *s);

/**
 * @brief Check if a feature is supported/has been negociated with the server
 *
 * @param s The session object
 * @param what Which features to check ? @see smb_session_supports_what
 *
 * @return 0 if the feature is not supported, something else otherwise
 */
int             smb_session_supports(smb_session *s, int what);

#endif
