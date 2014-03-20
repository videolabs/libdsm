// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_NETBIOS_NS_H_
#define __BDSM_NETBIOS_NS_H_

#include "bdsm/netbios_defs.h"

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

typedef struct      netbios_ns_s {
  int                 socket;
  struct sockaddr_in  addr;
  uint16_t            last_trn_id;  // Last transaction id used;
}                   netbios_ns_t;

netbios_ns_t  *netbios_ns_new();
void          netbios_ns_destroy(netbios_ns_t *ns);
uint32_t      netbios_ns_resolve(netbios_ns_t *ns, const char *name);

#endif
