// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <string.h>

#include "bdsm/context.h"

bdsm_context_t    *bdsm_context_new()
{
  bdsm_context_t  *ctx;

  ctx = malloc(sizeof(bdsm_context_t));
  memset((void *)ctx, 0, sizeof(bdsm_context_t));

  //if (BDSM_DEBUG)
  //  event_enable_debug_mode();

  //ctx->event_base = event_base_new();
  if (!(ctx->ns = netbios_ns_new()))
  {
    free(ctx);
    return (0);
  }

  return (ctx);
}

void              bdsm_context_close(bdsm_context_t *ctx)
{
  if (!ctx)
    return;

  // if (ctx->event_base)
  //   event_base_free(ctx->event_base);

  free(ctx);
}
