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
#include <string.h>
#include <assert.h>

#include "bdsm/context.h"

bdsm_context_t    *bdsm_context_new()
{
  bdsm_context_t  *ctx;

  ctx = calloc(1, sizeof(bdsm_context_t));
  assert(ctx != NULL);

  if (!(ctx->ns = netbios_ns_new()))
  {
    free(ctx);
    return (0);
  }

  return (ctx);
}

void              bdsm_context_destroy(bdsm_context_t *ctx)
{
  if (!ctx)
    return;

  free(ctx);
}
