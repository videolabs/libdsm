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
 * @internal
 * @file smb_utils.h
 * @brief SMB helper functions
 */

#ifndef __BDSM_SMB_UTILS_H_
#define __BDSM_SMB_UTILS_H_

#include <stdint.h>

/**
 * @internal
 * @brief Converts a string from current locale encoding to UCS2-LE
 * @details It allocates the output string on the heap, you have to free it !
 *
 * @param[in] src The input string. It is considere to be in the current locale
 * @param[in] src_len The length in byte of the input string (strlen will do
 *  the job for utf-8 or single byte encoding)
 * @param[out] dst Will be set to the newly allocated encoded string, you have
 *  to free() it after usage unless the function returns 0, in that case, it'll
 *  be set to NULL
 * @return The size of the encoded string in bytes
 */
size_t      smb_to_utf16(const char *src, size_t src_len, char **dst);

/**
 * @internal
 * @brief Converts from UCS2-LE to local encoding (fetched using setlocale())
 * @details It returns a newly allocated string that you need to free
 * after usage
 *
 * @param[in] src The UCS2-LE string to be converved to local encoding
 * @param[in] src_len The size in bytes of src
 * @param[out] dst will be set to the newly allocated and converted string.
 *   You have to free() it unless the function returns 0, in that case, it'll
 *   be set to NULL
 * @return The size of the decoded string in bytes
 */
size_t      smb_from_utf16(const char *src, size_t src_len, char **dst);

#endif
