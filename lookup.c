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
  bdsm_context_t      *ctx;
  char                name[NETBIOS_NAME_LENGTH + 2];
  struct in_addr      addr;

  ctx = bdsm_context_new();
  assert(ctx);

  if (ac != 2)
  {
    fprintf(stderr, "%s usage: %s NBT_NAME\n", av[0], av[0]);
    fprintf(stderr, "Print the ip for this netbios name\n");
    exit(1);
  }

  addr.s_addr = netbios_ns_resolve(ctx->ns, av[1], NETBIOS_FILESERVER);
  if (!addr.s_addr)
  {
    fprintf(stderr, "Unable to perform name resolution for %s\n", av[1]);
    exit(42);
  }

  printf("%s's IP address is : %s\n", av[1], inet_ntoa(addr));

  return (0);
}
