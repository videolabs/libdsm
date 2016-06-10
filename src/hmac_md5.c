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

#include "config.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hmac_md5.h"

unsigned char *HMAC_MD5(const void *key, size_t key_len, const void *msg,
                        size_t msg_len, void *hmac)
{
    static uint8_t  hmac_static[16];

    uint8_t         key_pad[64], o_key_pad[64], i_key_pad[64], kcat[80];
    uint8_t         *cat, *out;
    MD5_CTX         ctx;

    assert(key != NULL && msg != NULL);

    // This is Microsoft variation of HMAC_MD5 for NTLMv2
    // It seems they truncate over-sized keys instead of rehashing
    if (key_len > 64)
        key_len = 64;

    memcpy(key_pad, key, key_len);
    if (key_len < 64)
        memset(key_pad + key_len, 0, 64 - key_len);

    // Compute the o/i XORed padded keys
    for (unsigned i = 0; i < 64; i++)
    {
        o_key_pad[i] = 0x5c ^ key_pad[i];
        i_key_pad[i] = 0x36 ^ key_pad[i];
    }

    // Concatenate inner padded key with message
    cat = malloc(msg_len + 64);
    if (!cat)
        return NULL;
    memcpy(cat, i_key_pad, 64);
    memcpy(cat + 64, msg, msg_len);

    // MD5 of the result
    MD5_Init(&ctx);
    MD5_Update(&ctx, cat, msg_len + 64);
    MD5_Final(key_pad, &ctx);
    free(cat);

    memcpy(kcat, o_key_pad, 64);
    memcpy(kcat + 64, key_pad, 16);

    if (hmac != NULL)
        out = hmac;
    else
        out = hmac_static;

    MD5_Init(&ctx);
    MD5_Update(&ctx, kcat, 80);
    MD5_Final(out, &ctx);

    return out;
}
