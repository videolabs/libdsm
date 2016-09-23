/*****************************************************************************
 *  __________________    _________  _____            _____  .__         ._.
 *  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
 *   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
 *   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
 *   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
 *          \/        \/        \/        \/  )/           \/        \/   \/
 *
 * This file is part of liBDSM. Copyright © 2014-2015 VideoLabs SAS
 *
 * Author: Julien 'Lta' BALLET <contact@lta.io>
 *
 * liBDSM is released under LGPLv2.1 (or later) and is also available
 * under a commercial license.
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef __BDSM_SMB_SESSION_H_
#define __BDSM_SMB_SESSION_H_


#include "bdsm/smb_defs.h"
#include "bdsm/smb_types.h"

/**
 * @file smb_session.h
 * @brief Functions to connect and authenticate to an SMB server
 */

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
 * @brief Set the credentials for this session.
 * @details Any of the params except s can be NULL.
 *
 * @param s The session object.
 * @param domain Domain to authenticate on. Often it's the same as netbios host.
 * @param login The user to login as.
 * @param password the user's password.
 */
void            smb_session_set_creds(smb_session *s, const char *domain,
                                      const char *login, const char *password);
#define SMB_CREDS_MAXLEN 128



/**
 * @brief Establish a connection and negotiate a session protocol with a remote
 * host
 * @details You have to provide both the ip and the name. This is a constraint
 * of Netbios, which requires you to know its name before he accepts to speak
 * with you.
 *
 * @param s A session object.
 * @param hostname The ASCII netbios name, the name type will be coerced to <20>
 * since libdsm is about reading files
 * @param ip The ip of the machine to connect to (in network byte order)
 * @param transport The type of transport used, it could be SMB_TRANSPORT_TCP
 * or SMB_TRANSPORT_NBT (Netbios over TCP, ie legacy)
 * @return 0 on success or a DSM error code in case of error
 */
int             smb_session_connect(smb_session *s, const char *hostname,
                                    uint32_t ip, int transport);

/**
 * @brief Authenticate on the remote host with the provided credentials
 * @details Can be called if session state is SMB_STATE_DIALECT_OK.
 * If successfull, session state transition to SMB_STATE_SESSION_OK
 * Provides the credentials with smb_session_set_creds.
 *
 * @param s The session object.
 *
 * @return 0 on success or a DSM error code in case of error. Success doesn't
 * mean you are logged in with the user you requested. If guest are activated
 * on the remote host, when login fails, you are logged in as 'Guest'. Failure
 * might also indicate you didn't supplied all the credentials
 */
int             smb_session_login(smb_session *s);

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

/**

 * @brief Get the last NT_STATUS
 * @details Valid only if a smb_ function returned the DSM_ERROR_NT error.
 *
 * @param s The session object
 */
uint32_t        smb_session_get_nt_status(smb_session *s);


#endif
