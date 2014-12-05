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
void    smb_buffer_alloc(smb_buffer *buf, size_t size);

/**
 * @brief Allocate a size long memory area from the stack and place it in
 *  the buffer structure
 */
#define smb_buffer_alloca(buf, sz)    \
  (buf)->data = alloca(sz);           \
  (buf)->size = sz;                   \
  assert((buf)->data != NULL);


/**
 * @brief Free the data of this buffer if necessary
 *
 * @param buf Pointer to a buffer to free
 */
void    smb_buffer_free(smb_buffer *buf);

#endif
