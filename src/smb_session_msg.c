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

#include <assert.h>

#include "bdsm/smb_session.h"

int             smb_session_send_msg(smb_session *s, smb_message *msg)
{
    size_t        pkt_sz;

    assert(s != NULL && s->state >= SMB_STATE_NETBIOS_OK);
    assert(s->transport.session != NULL);
    assert(msg != NULL && msg->packet != NULL);

    msg->packet->header.flags   = 0x18;
    msg->packet->header.flags2  = 0xc843;
    // msg->packet->header.flags2  = 0xc043; // w/o extended security;
    msg->packet->header.uid = s->srv.uid;

    s->transport.pkt_init(s->transport.session);

    pkt_sz = sizeof(smb_packet) + msg->cursor;
    if (!s->transport.pkt_append(s->transport.session, (void *)msg->packet, pkt_sz))
        return (0);
    if (!s->transport.send(s->transport.session))
        return (0);

    return (1);
}

ssize_t         smb_session_recv_msg(smb_session *s, smb_message *msg)
{
    void                      *data;
    ssize_t                   payload_size;

    assert(s != NULL && s->transport.session != NULL);

    payload_size = s->transport.recv(s->transport.session, &data);
    if (payload_size <= 0)
        return (0);

    if (msg != NULL)
    {
        msg->packet = (smb_packet *)data;
        msg->payload_size = payload_size - sizeof(smb_header);
        msg->cursor       = 0;
    }

    return (payload_size - sizeof(smb_header));
}
