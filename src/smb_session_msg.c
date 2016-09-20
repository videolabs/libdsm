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

#include "smb_session.h"
#include "smb_message.h"

int             smb_session_send_msg(smb_session *s, smb_message *msg)
{
    size_t        pkt_sz;

    assert(s != NULL);
    assert(s->transport.session != NULL);
    assert(msg != NULL && msg->packet != NULL);

    msg->packet->header.flags   = 0x18;
    msg->packet->header.flags2  = 0xc843;
    // msg->packet->header.flags2  = 0xc043; // w/o extended security;
    msg->packet->header.uid = s->srv.uid;

    s->transport.pkt_init(s->transport.session);

    pkt_sz = sizeof(smb_packet) + msg->cursor;
    if (!s->transport.pkt_append(s->transport.session, (void *)msg->packet, pkt_sz))
        return 0;
    if (!s->transport.send(s->transport.session))
        return 0;

    return 1;
}

size_t          smb_session_recv_msg(smb_session *s, smb_message *msg)
{
    void                      *data;
    ssize_t                   payload_size;

    assert(s != NULL && s->transport.session != NULL);

    payload_size = s->transport.recv(s->transport.session, &data);
    if (payload_size <= 0)
        return 0;

    if ((size_t)payload_size < sizeof(smb_header))
        return 0;

    if (msg != NULL)
    {
        msg->packet = (smb_packet *)data;
        msg->payload_size = payload_size - sizeof(smb_header);
        msg->cursor       = 0;
    }

    return payload_size - sizeof(smb_header);
}
