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

/**
 * @file smb_stat.h
 * @brief File status / listing
 */

#ifndef __BDSM_SMB_STAT_H_
#define __BDSM_SMB_STAT_H_

#include "bdsm/smb_defs.h"
#include "bdsm/smb_session.h"

/// smb_stat_get() OP: Get file size
#define SMB_STAT_SIZE         0
/// smb_stat_get() OP: Get file allocation size (Size on disk)
#define SMB_STAT_ALLOC_SIZE   1
/// smb_stat_get() OP: 0 -> not a directory, != 0 -> directory
#define SMB_STAT_ISDIR        2
/// smb_stat_get() OP: Get file creation time
#define SMB_STAT_CTIME        3
/// smb_stat_get() OP: Get file last access time
#define SMB_STAT_ATIME        4
/// smb_stat_get() OP: Get file last write time
#define SMB_STAT_WTIME        5
/// smb_stat_get() OP: Get file last moditification time
#define SMB_STAT_MTIME        6

/**
 * @brief Returns infos about files matching a pattern
 * @details This functions uses the FIND_FIRST2 SMB operations to list files
 * matching a certain pattern. It's basically used to list folder contents
 *
 * @param s The session object
 * @param tid The share inside of which we want to find files obtained by
 * smb_tree_connect()
 * @param pattern The pattern to match files. '\\*' will list all the files at
 * the root of the share. '\\afolder\\*' will list all the files inside of the
 * 'afolder' directory.
 * @return An opaque list of smb_stat or NULL in case of error
 */
smb_stat_list   smb_find(smb_session *s, smb_tid tid, const char *pattern);

/**
 * @brief Get the status of a file from it's path inside of a share
 *
 * @param s The session object
 * @param tid The tree id of a share obtained by smb_tree_connect()
 * @param path The full path of the file relative to the root of the share
 * (e.g. '\\folder\\file.ext')
 *
 * @return An opaque smb_stat or NULL in case of error. You need to
 * destory this object with smb_stat_destroy after usage.
 */
smb_stat        smb_fstat(smb_session *s, smb_tid tid, const char *path);

/**
 * @brief Get the status of an open file from it's file descriptor
 * @details The file status will be those at the time of open
 *
 * @param s The session object
 * @param fd The smb_fd from which you want infos/status
 *
 * @return An opaque smb_stat or NULL in case of error. You don't own
 * this object memory, and then don't have to destory it
 */
smb_stat        smb_stat_fd(smb_session *s, smb_fd fd);

/**
 * @brief Clear a smb_stat object, reclaiming its memory
 *
 * @param stat A smb_stat object returned by smb_fstat. 
 */
void            smb_stat_destroy(smb_stat stat);

/**
 * @brief Get the number of item in a smb_stat_list file info
 *
 * @param list The list you want the length of
 * @return The length of the list. It returns 0 if the list is invalid
 */
size_t            smb_stat_list_count(smb_stat_list list);

/**
 * @brief Get the element at the given position.
 *
 * @param list A stat list
 * @param index The position of the element you want.
 *
 * @return An opaque smb_stat or NULL in case of error
 */
smb_stat        smb_stat_list_at(smb_stat_list list, size_t index);

/**
 * @brief Destroy and release a list of file stat returned by smb_find
 *
 * @param list The stat_list to free
 */
void            smb_stat_list_destroy(smb_stat_list list);

/**
 * @brief Get the name of the file from its status
 *
 * @param info A file status
 * @return A null-terminated string in you current locale encoding or NULL.
 */
const char        *smb_stat_name(smb_stat info);

/**
 * @brief Get a file attribute
 * @details This function is a getter that allow you to retrieve various
 * informations about a file on a smb_stat object. You can get its size,
 * various timestamps, etc.
 *
 * @param info The smb_stat object to get info from.
 * @param what This parameter tells the functions which information to get, can
 * be one of #SMB_STAT_SIZE, #SMB_STAT_ALLOC_SIZE, #SMB_STAT_ISDIR,
 * #SMB_STAT_CTIME, #SMB_STAT_ATIME, #SMB_STAT_MTIME, #SMB_STAT_WTIME.
 *
 * @return The meaning of the returned value depends on the 'what' parameter.
 * See each item documentation.
 */
uint64_t          smb_stat_get(smb_stat info, int what);

#endif
