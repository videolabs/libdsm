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
 * Author: Sylver Bruneau <sylver.bruneau@gmail.com>
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

/**
 * @file smb_dir.h
 * @brief Directory operations
 */

#ifndef __BDSM_SMB_DIR_H_
#define __BDSM_SMB_DIR_H_

#include "smb_session.h"


/**
 * @brief remove a directory on a share.
 * @details Use this function to delete an empty directory
 *
 * @param s The session object
 * @param tid The tid of the share the file is in, obtained via smb_tree_connect()
 * @param path The path of the file to delete
 * @return 0 on success or a DSM error code in case of error
 */
int smb_directory_rm(smb_session *s, smb_tid tid, const char *path);

/**
 * @brief create a directory on a share.
 * @details Use this function to create a directory
 *
 * @param s The session object
 * @param tid The tid of the share the file is in, obtained via smb_tree_connect()
 * @param path The path of the directory to create
 * @return 0 on success or a DSM error code in case of error
 */
int smb_directory_create(smb_session *s, smb_tid tid, const char *path);

#endif
