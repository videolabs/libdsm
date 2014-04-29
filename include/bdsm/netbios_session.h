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

#ifndef __BDSM_NETBIOS_SESSION_H_
#define __BDSM_NETBIOS_SESSION_H_

#include <stdint.h>
#include <stddef.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "bdsm/netbios_defs.h"

#define NETBIOS_SESSION_NEW         0
#define NETBIOS_SESSION_CONNECTING  1
#define NETBIOS_SESSION_CONNECTED   2
#define NETBIOS_SESSION_ERROR       -1
#define NETBIOS_SESSION_REFUSED     -2

typedef struct              netbios_session_s {
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
int               netbios_session_connect(struct in_addr *addr,
                                          netbios_session *s,
                                          const char *name);
void              netbios_session_packet_init(netbios_session *s);
int               netbios_session_packet_append(netbios_session *s,
                                                const char *data, size_t size);
int               netbios_session_packet_send(netbios_session *s);
ssize_t           netbios_session_packet_recv(netbios_session *s, void **data);

#endif
