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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "netbios_utils.h"

static short  nibble_encode(char c)
{
    short n1, n2;

    n1 = (toupper(c) >> 4)   + 'A';
    n2 = (toupper(c) & 0x0F) + 'A';

    return ((n1 << 8) | n2);
}

static char   nibble_decode(char c1, char c2)
{
    return (((c1 - 'A') << 4) + (c2 - 'A'));
}


// name must be null-terminated, it will be truncated if >= 16 chars
// encoded name must the 33bytes long at least, will be null-terminated
void  netbios_name_level1_encode(const char *name, char *encoded_name,
                                 unsigned type)
{
    size_t    name_length = strlen(name);

    if (name_length > NETBIOS_NAME_LENGTH)
        name_length = NETBIOS_NAME_LENGTH;

    for (unsigned int i = 0; i < NETBIOS_NAME_LENGTH; i++)
    {
        if (i < name_length)
        {
            encoded_name[2 * i]     = nibble_encode(name[i]) >> 8;
            encoded_name[2 * i + 1] = nibble_encode(name[i]) & 0x00FF;
        }
        else
        {
            encoded_name[2 * i]     = 'C';
            encoded_name[2 * i + 1] = 'A';
        }
    }

    encoded_name[30] = nibble_encode(type) >> 8;
    encoded_name[31] = nibble_encode(type) &  0x00FF;
    encoded_name[32] = '\0';
}

// encoded_name: 33bytes long
// name: at least 16bytes long (the string will be null-terminated)
void  netbios_name_level1_decode(const char *encoded_name, char *name)
{
    for (unsigned int i = 0; i < NETBIOS_NAME_LENGTH; i++)
    {
        name[i] = nibble_decode(encoded_name[2 * i], encoded_name[2 * i + 1]);
    }
    name[NETBIOS_NAME_LENGTH] = 0;
}

// XXX: Supports domain
char  *netbios_name_encode(const char *name, char *domain,
                           unsigned type)
{
    (void)domain;  // Unused yet

    size_t    encoded_size = 34; // length byte + 32 bytes for encoded name + terminator
    char      *encoded_name;

    if (!name)
        return NULL;

    encoded_name = malloc(encoded_size);
    if (!encoded_name)
        return NULL;
    encoded_name[0] = 32; // length of the field;
    netbios_name_level1_encode(name, encoded_name + 1, type);
    encoded_name[33] = 0;

    //printf("Encoded name (l2): %s.\n", encoded_name);

    return encoded_name;
}

int             netbios_name_decode(const char *encoded_name,
                                    char *name, char **domain)
{
    (void)domain;  // Unused yet

    size_t  encoded_length;

    if (!encoded_name || !name)
        return -1;

    encoded_length =  strlen(encoded_name);

    // XXX Support domain name :p
    if (encoded_length != 33)
        return -1;

    netbios_name_level1_decode(encoded_name + 1, name);
    name[33] = '\0';
    return 32;
}


