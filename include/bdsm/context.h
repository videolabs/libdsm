// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_CONTEXT_H_
#define __BDSM_CONTEXT_H_

//#include <event2/event.h>

#include "bdsm/netbios_ns.h"

typedef struct      bdsm_context_s {
  //struct event_base   *event_base;
  netbios_ns_t      *ns;
}                   bdsm_context_t;

bdsm_context_t    *bdsm_context_new();
void              bdsm_context_close(bdsm_context_t *ctx);

#endif
