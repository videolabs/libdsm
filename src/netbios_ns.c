// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include "bdsm/netbios_ns.h"

static int    ns_open_socket(netbios_ns_t *ns)
{
  int sock_opt;

  if ((ns->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    goto error;

  sock_opt = 1;
  if (setsockopt(ns->socket, SOL_SOCKET, SO_BROADCAST,
      (void *)&sock_opt, sizeof(sock_opt)) < 0)
    goto error;

  sock_opt = 0;
  if (setsockopt(ns->socket, IPPROTO_IP, IP_MULTICAST_LOOP,
      (void *)&sock_opt, sizeof(sock_opt)) < 0)
    goto error;

  ns->addr.sin_family       = AF_INET;
  ns->addr.sin_port         = htons(0);
  ns->addr.sin_addr.s_addr  = 0;
  if (bind(ns->socket, (struct sockaddr *)&ns->addr, sizeof(ns->addr)) < 0)
    goto error;

  return (1);

  error:
    perror("netbios_ns_new, open_socket: ");
    return (0);
}

netbios_ns_t  *netbios_ns_new()
{
  netbios_ns_t  *ns;

  ns = malloc(sizeof(netbios_ns_t));
  memset((void *)ns, 0, sizeof(netbios_ns_t));

  if (!ns_open_socket(ns))
  {
    netbios_ns_destroy(ns);
    return (0);
  }

  ns->last_trn_id = rand();

  return (ns);
}

void          netbios_ns_destroy(netbios_ns_t *ns)
{
  if (!ns)
    return;

  close(ns->socket);
  free(ns);
}

uint32_t      netbios_ns_resolve(netbios_ns_t *ns, const char *name)
{

}
