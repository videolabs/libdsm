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

#ifndef _SMB_SESSION_H_
#define _SMB_SESSION_H_

#include "smb_types.h"
#include "bdsm/smb_session.h"

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

/* Our reception buffer grows as necessary, so we can put the max here */
#define SMB_SESSION_MAX_BUFFER (0xffff)

bool smb_session_check_nt_status(smb_session *s, smb_message *msg);

#endif
