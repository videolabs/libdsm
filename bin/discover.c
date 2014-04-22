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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "bdsm.h"

int main(int ac, char **av)
{
  netbios_ns_t        *ns;
  netbios_ns_entry_t  *entry;

  ns = netbios_ns_new();

  if (!netbios_ns_discover(ns))
  {
    fprintf(stderr, "Error while discovering local network\n");
    exit(42);
  }

  for (int i = 0; i < netbios_ns_entry_count(ns); i++)
  {
    struct in_addr addr;

    entry       = netbios_ns_entry_at(ns, i);
    addr.s_addr = netbios_ns_entry_ip(entry);

    printf("Ip: %s, name: %s<%x> \n",
      inet_ntoa(addr),
      netbios_ns_entry_name(entry),
      netbios_ns_entry_type(entry));
  }

  return (0);
}
