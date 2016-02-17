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

#ifndef _NETBIOS_QUERY_H_
#define _NETBIOS_QUERY_H_

#include <stdint.h>
#include <stddef.h>

#include "netbios_defs.h"

typedef struct              netbios_query_s
{
    size_t                      payload_size;
    size_t                      cursor;
    netbios_query_packet      *packet;
}                           netbios_query;

netbios_query   *netbios_query_new(size_t payload_size, int is_query,
                                   char opcode);
void              netbios_query_destroy(netbios_query *q);
void              netbios_query_set_flag(netbios_query *q,
        uint16_t flag, int value);
int               netbios_query_append(netbios_query *q, const char *data,
                                       size_t data_size);

void              netbios_query_print(netbios_query *q);


#endif
