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

#include "bdsm/smb_transport.h"
#include "bdsm/netbios_session.h"

// XXX: This can be simplified, since we have only one function that differs

int               transport_connect_nbt(struct in_addr *addr,
                                        netbios_session *s,
                                        const char *name)
{
    return (netbios_session_connect(addr, s, name, 0));
}

int               transport_connect_tcp(struct in_addr *addr,
                                        netbios_session *s,
                                        const char *name)
{
    return (netbios_session_connect(addr, s, name, 1));
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
