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
 * @file smb_file.h
 * @brief File operations
 */

#ifndef __BDSM_SMB_FILE_H_
#define __BDSM_SMB_FILE_H_

#include "bdsm/smb_session.h"

/// Set the read pointer at the given position
#define SMB_SEEK_SET 0
/// Adjusts the read pointer relatively to the actual position
#define SMB_SEEK_CUR 1

/**
 * @brief Open a file on a share.
 * @details Use this function to obtain an smb_fd, necesary for file operations
 *
 * @param s The session object
 * @param tid The tid of the share the file is in, obtained via smb_tree_connect()
 * @param path The path of the file to open
 * @param mod The access modes requested (example: #SMB_MOD_RO)
 * @return A smb file description that can be use for further file operations
 * or 0 in case of error
 *
 * @see smb_tree_connect
 */
smb_fd    smb_fopen(smb_session_t *s, smb_tid tid, const char *path,
                    uint32_t mod);

/**
 * @brief Close an open file
 * @details The smb_fd is invalidated and MUST not be use it anymore. You can
 * give it the 0 value.
 *
 * @param s The session object
 * @param fd The SMB file descriptor
 */
void      smb_fclose(smb_session_t *s, smb_fd fd);

/**
 * @brief Read from an open file
 * @details The semantics is basically the same that the unix read() one.
 * At most 'buf_size' bytes are read from the current seek offset and copied into
 * the memory pointed by 'buf' from the open file represented by the smb file
 * descriptor 'fd'.
 *
 * @param[in] s The session object
 * @param[in] fd [description]
 * @param[out] buf [description]
 * @param[in] buf_size [description]
 * @return The number of bytes read or -1 in case of error.
 */
ssize_t   smb_fread(smb_session_t *s, smb_fd fd, void *buf, size_t buf_size);

/**
 * @brief Sets/Moves/Get the read pointer for a given file
 * @details The behavior of this function is the same as the Unix fseek()
 * function, except the SEEK_END argument isn't supported.
 *
 * This functions adjust the read the read pointer depending on the value of
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
 * @return The current read pointer position
 */
ssize_t   smb_fseek(smb_session_t *s, smb_fd fd, ssize_t offset, int whence);

#endif
