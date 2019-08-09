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
 * @file smb_file.h
 * @brief File operations
 */

#ifndef __BDSM_SMB_FILE_H_
#define __BDSM_SMB_FILE_H_

#include "smb_session.h"


/**
 * @brief Open a file on a share.
 * @details Use this function to obtain an smb_fd, necesary for file operations
 *
 * @param s The session object
 * @param tid The tid of the share the file is in, obtained via smb_tree_connect()
 * @param path The path of the file to open
 * @param mod The access modes requested (example: #SMB_MOD_RO)
 * @param fd The pointer to the smb file description that can be used for
 * further file operations
 * @return 0 on success or a DSM error code in case of error
 *
 * @see smb_tree_connect
 */
int       smb_fopen(smb_session *s, smb_tid tid, const char *path,
                    uint32_t mod, smb_fd *fd);

/**
 * @brief Close an open file
 * @details The smb_fd is invalidated and MUST not be use it anymore. You can
 * give it the 0 value.
 *
 * @param s The session object
 * @param fd The SMB file descriptor
 */
void      smb_fclose(smb_session *s, smb_fd fd);

/**
 * @brief Read from an open file
 * @details The semantics is basically the same that the unix read() one.
 * At most 'buf_size' bytes are read from the current seek offset and copied into
 * the memory pointed by 'buf' from the open file represented by the smb file
 * descriptor 'fd'.
 *
 * @param[in] s The session object
 * @param[in] fd [description]
 * @param[out] buf can be NULL in order to skip buf_size bytes
 * @param[in] buf_size [description]
 * @return The number of bytes read or -1 in case of error.
 */
ssize_t   smb_fread(smb_session *s, smb_fd fd, void *buf, size_t buf_size);

/**
 * @brief Write to an open file
 * @details At most 'buf_size' bytes from memory pointed by 'buf' are written
 * to the current seek offset of the open file represented by the smb file
 * descriptor 'fd'.
 *
 * @param[in] s The session object
 * @param[in] fd [description]
 * @param[out] buf [description]
 * @param[in] buf_size [description]
 * @return The number of bytes written or -1 in case of error.
 */
ssize_t   smb_fwrite(smb_session *s, smb_fd fd, void *buf, size_t buf_size);

/**
 * @brief Sets/Moves/Get the read/write pointer for a given file
 * @details The behavior of this function is the same as the Unix fseek()
 * function, except the SEEK_END argument isn't supported.
 *
 * This functions adjust the read/write pointer depending on the value of
 * offset and whence.
 *
 * - If whence == #SMB_SEEK_SET, the read pointer is set at 'offset'
 * - If whence == #SMB_SEEK_CUR, the read pointer is adjusted by 'offset'
 *
 * @param s The session object
 * @param fd The file descriptors for which the read pointer is to be adjusted
 * @param offset Set/Adjust quantity
 * @param whence Which action to perform. Supported operations are
 * #SMB_SEEK_SET and #SMB_SEEK_CUR
 * @return The current read pointer position or -1 on error
 */
ssize_t   smb_fseek(smb_session *s, smb_fd fd, off_t offset, int whence);

/**
 * @brief remove a file on a share.
 * @details Use this function to delete a file
 *
 * @param s The session object
 * @param tid The tid of the share the file is in, obtained via smb_tree_connect()
 * @param path The path of the file to delete
 * @return 0 if delete OK or "NT" error code
 */
int  smb_file_rm(smb_session *s, smb_tid tid, const char *path);

/**
 * @brief move/rename a file/directory on a share.
 * @details Use this function to move and/or rename a file/directory
 *
 * @param s The session object
 * @param tid The tid of the share the file is in, obtained via smb_tree_connect()
 * @param old_path The current path of the file/directory to move/rename
 * @param new_path The new path of the file/directory
 * @return 0 if move OK or -1 in case of error
 */
int       smb_file_mv(smb_session *s, smb_tid tid, const char *old_path, const char *new_path);

#endif
