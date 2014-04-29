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

  tr->new           = netbios_session_new;
  tr->connect       = transport_connect_nbt;
  tr->destroy       = netbios_session_destroy;
  tr->pkt_init      = netbios_session_packet_init;
  tr->pkt_append    = netbios_session_packet_append;
  tr->send          = netbios_session_packet_send;
  tr->recv          = netbios_session_packet_recv;
}

void              smb_transport_tcp(smb_transport *tr)
{
  assert(tr != NULL);

  tr->new           = netbios_session_new;
  tr->connect       = transport_connect_tcp;
  tr->destroy       = netbios_session_destroy;
  tr->pkt_init      = netbios_session_packet_init;
  tr->pkt_append    = netbios_session_packet_append;
  tr->send          = netbios_session_packet_send;
  tr->recv          = netbios_session_packet_recv;
}
