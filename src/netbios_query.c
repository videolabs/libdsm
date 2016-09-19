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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef _WIN32
# include <winsock2.h>
#endif

#include "netbios_query.h"

netbios_query   *netbios_query_new(size_t payload_size,
                                   int is_query, char opcode)
{
    netbios_query *q;

    q = calloc(1, sizeof(netbios_query));
    if (!q)
        return NULL;

    q->packet = calloc(1, sizeof(netbios_query_packet) + payload_size);
    if (!q->packet) {
        free(q);
        return NULL;
    }

    q->payload_size = payload_size;

    q->packet->flags  = htons(opcode << 11);
    netbios_query_set_flag(q, NETBIOS_FLAG_QUERY, !is_query);

    return q;
}

void              netbios_query_destroy(netbios_query *q)
{
    assert(q);

    free(q->packet);
    free(q);
}

void              netbios_query_set_flag(netbios_query *q,
        uint16_t flag, int value)
{
    assert(q && q->packet);

    if (value)
        q->packet->flags = htons(ntohs(q->packet->flags) | flag);
    else
        q->packet->flags = htons(ntohs(q->packet->flags) & ~flag);
}

void              netbios_query_print(netbios_query *q)
{
    assert(q && q->packet);

    printf("--- netbios_query dump :\n");
    printf("payload = %zu, cursor = %zu.\n", q->payload_size, q->cursor);
    printf("Transaction id = %u.\n", q->packet->trn_id);

    printf("-------------------------\n");
    for (unsigned i = 0; i < sizeof(netbios_query_packet) + q->cursor; i++)
    {
        char c;
        if ((i % 8) == 0 && i != 0)
            printf("\n");
        if ((i % 8) == 0)
            printf("0x");

        c = ((char *)q->packet)[i];
        printf("%.2hhX ", c);
    }
    printf("\n");
    printf("-------------------------\n");
}

int               netbios_query_append(netbios_query *q, const char *data,
                                       size_t data_size)
{
    assert(q && q->packet);

    if (q->payload_size - q->cursor < data_size)
        return -1;

    memcpy(((char *)&q->packet->payload) + q->cursor, data, data_size);
    q->cursor += data_size;
    return 0;
}
