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
  netbios_ns        *ns;
  struct in_addr      addr;
  const char          *name;

  ns = netbios_ns_new();

  if (ac != 2)
  {
    fprintf(stderr, "%s usage: %s a.b.c.d\n", av[0], av[0]);
    fprintf(stderr, "Print the netbios name for this IP address\n");
    exit(1);
  }

  inet_aton(av[1], &addr);
  if ((name = netbios_ns_inverse(ns, addr.s_addr)) == NULL)
  {
    fprintf(stderr, "Unable to perform inverse name resolution for %s\n", av[1]);
    exit(42);
  }

  printf("%s\n", name);
  netbios_ns_destroy(ns);
  return (0);
}
