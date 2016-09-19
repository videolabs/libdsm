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

#include "smb_transport.h"
#include "netbios_session.h"

// XXX: This can be simplified, since we have only one function that differs

int               transport_connect_nbt(uint32_t ip,
                                        netbios_session *s,
                                        const char *name)
{
    return netbios_session_connect(ip, s, name, 0);
}

int               transport_connect_tcp(uint32_t ip,
                                        netbios_session *s,
                                        const char *name)
{
    return netbios_session_connect(ip, s, name, 1);
}

void              smb_transport_nbt(smb_transport *tr)
{
    assert(tr != NULL);

    // Sorry for the dirty cast.
    tr->new           = (void *)netbios_session_new;
    tr->connect       = (void *)transport_connect_nbt;
    tr->destroy       = (void *)netbios_session_destroy;
    tr->pkt_init      = (void *)netbios_session_packet_init;
    tr->pkt_append    = (void *)netbios_session_packet_append;
    tr->send          = (void *)netbios_session_packet_send;
    tr->recv          = (void *)netbios_session_packet_recv;
}

void              smb_transport_tcp(smb_transport *tr)
{
    assert(tr != NULL);

    tr->new           = (void *)netbios_session_new;
    tr->connect       = (void *)transport_connect_tcp;
    tr->destroy       = (void *)netbios_session_destroy;
    tr->pkt_init      = (void *)netbios_session_packet_init;
    tr->pkt_append    = (void *)netbios_session_packet_append;
    tr->send          = (void *)netbios_session_packet_send;
    tr->recv          = (void *)netbios_session_packet_recv;
}
