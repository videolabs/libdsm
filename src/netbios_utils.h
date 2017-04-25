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

#ifndef _NETBIOS_UTILS_H_
#define _NETBIOS_UTILS_H_

#include "netbios_defs.h"

/**
 * @internal
 * @brief Encode the machine name part of a netbios name
 *
 * @param name The string to encode. Only the first 15 chars are used
 * @param encoded_name This string will be filled with the encoded name. Must
 *   be 33 bytes long
 * @param type In netbios, the name encodes the service type,
 *   like NETBIOS_FILESERVER
 */
void  netbios_name_level1_encode(const char *name, char *encoded_name,
                                 unsigned type);
/**
 * @internal
 * @brief Decode the machine part of a netbios name
 */
void  netbios_name_level1_decode(const char *encoded_name, char *name);

// XXX: domain support is not implemented

/**
 * @internal
 * @brief Encode a netbios machine name.
 *
 * @param name The name of the machine. Only the first 15 bytes are used
 * @param domain Not implemented yet
 * @param type The type of service we're talking to. Like NETBIOS_FILESERVER or
 *   NETBIOS_WORKSTATION
 */
char  *netbios_name_encode(const char *name, char *domain,
                           unsigned type);

/**
 * @internal
 * @brief Decode a netbios machine name.
 *
 * @param encoded_name The encoded name
 * @param name A 16 bytes long string (including null byte). Will be filled
 *   with the decoded name
 * @param domain Not Implemented yet
 */
int   netbios_name_decode(const char *encoded_name,
                          char *name, char **domain);

#endif
