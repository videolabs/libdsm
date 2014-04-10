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

#include "bdsm.h"

int main(int ac, char **av)
{
  bdsm_context_t      *ctx;
  netbios_ns_entry_t  *iter;

  ctx = bdsm_context_new();
  assert(ctx);

  if (!netbios_ns_discover(ctx->ns))
  {
    fprintf(stderr, "Error while discovering local network\n");
    exit(42);
  }

  iter = ctx->ns->entries;
  while (iter != NULL)
  {
    printf("Found %s\n", iter->name);
    iter = iter->next;
  }

  return (0);
}
