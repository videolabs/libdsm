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

#ifndef _NETBIOS_SESSION_H_
#define _NETBIOS_SESSION_H_

#include <stdint.h>
#include <stddef.h>

#if !defined _WIN32
# include <netinet/in.h>
#else
# include <winsock2.h>
#endif

#include "netbios_defs.h"

#define NETBIOS_SESSION_NEW         0
#define NETBIOS_SESSION_CONNECTING  1
#define NETBIOS_SESSION_CONNECTED   2
#define NETBIOS_SESSION_ERROR       -1
#define NETBIOS_SESSION_REFUSED     -2

typedef struct              netbios_session_s
{
    // The address of the remote peer;
    struct sockaddr_in          remote_addr;
    // The socket of the TCP connection to the HOST'
    int                         socket;
    // The current sessions state; See macro before (eg. NETBIOS_SESSION_ERROR)
    int                         state;
    // What is the size of the allocated payload;
    size_t                      packet_payload_size;
    // Where is the write cursor relative to the beginning of the payload
    size_t                      packet_cursor;
    // Our allocated packet, this is where the magic happen (both send and recv :)
    netbios_session_packet      *packet;
}                           netbios_session;


// Return NULL if unable to open socket/connect
netbios_session   *netbios_session_new(size_t buf_size);
void              netbios_session_destroy(netbios_session *);
int               netbios_session_connect(uint32_t ip,
        netbios_session *s,
        const char *name,
        int direct_tcp);
void              netbios_session_packet_init(netbios_session *s);
int               netbios_session_packet_append(netbios_session *s,
        const char *data, size_t size);
int               netbios_session_packet_send(netbios_session *s);
ssize_t           netbios_session_packet_recv(netbios_session *s, void **data);

#endif
