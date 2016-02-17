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
 * @file smb_types.h
 * @brief liBDSM types and structures
 */

#ifndef _SMB_BUFFER_H_
#define _SMB_BUFFER_H_

#include <stdlib.h>

/**
  * @struct smb_buffer
  * @brief Hold a pointer and the size of its data
  */
typedef struct
{
    void      *data;  /// Data pointed
    size_t    size;   /// Size in byte of the pointed
} smb_buffer;

/**
 * @brief Initialize a buffer structure. It'll contain nothing
 *
 * @param buf Pointer to a buffer to initialize
 */
void    smb_buffer_init(smb_buffer *buf, void *data, size_t size);

/**
 * @brief Allocate a size long memory area and place it in the buffer structure
 */
int     smb_buffer_alloc(smb_buffer *buf, size_t size);

/**
 * @brief Free the data of this buffer if necessary
 *
 * @param buf Pointer to a buffer to free
 */
void    smb_buffer_free(smb_buffer *buf);

#endif
