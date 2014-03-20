// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "bdsm/netbios_ns.h"
#include "bdsm/netbios_query.h"
#include "bdsm/netbios_utils.h"

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

  assert(ns = malloc(sizeof(netbios_ns_t)));
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
  netbios_query_t     *q;
  char                *encoded_name;
  char                footer[4] = { 0x00, 0x20, 0x00, 0x01 };
  char                recv_buffer[512]; // Hu ?

  struct sockaddr_in  bcast_addr;
  ssize_t             sent;
  ssize_t             recv;


  assert(ns);
  encoded_name = netbios_name_encode(name, 0, NETBIOS_WORKSTATION);
  if (!encoded_name)
    return (0);
  //q = netbios_query_new(ns->last_trn_id, 34, 1, NETBIOS_OPCODE_NAME_QUERY);
  //printf("Encoded name (l2): %s.\n", encoded_name);
  q = netbios_query_new(ns->last_trn_id, 34 + 4, 1, NETBIOS_OPCODE_NAME_QUERY);
  //printf("Encoded name (l2): %s.\n", encoded_name);
  netbios_query_set_flag(q, NETBIOS_FLAG_RECURSIVE, 1);
  netbios_query_set_flag(q, NETBIOS_FLAG_BROADCAST, 1);
  netbios_query_append(q, encoded_name, strlen(encoded_name) + 1);
  free(encoded_name);
  netbios_query_append(q, footer, 4);
  q->packet->queries = htons(1);
  //netbios_query_print(q);

  inet_aton("255.255.255.255", &(bcast_addr.sin_addr));
  bcast_addr.sin_family = AF_INET;
  bcast_addr.sin_port   = htons(137);

  sent = sendto(ns->socket, (void *)q->packet,
                sizeof(netbios_query_packet_t) + q->cursor, 0,
                (struct sockaddr *)&bcast_addr, sizeof(struct sockaddr_in));
  netbios_query_destroy(q);

  if (sent < 0)
    goto error;
  else if (BDSM_DEBUG)
    printf("netbios_ns_resolve, name query sent for '%s' !\n", name);

  recv = recvfrom(ns->socket, (void *)recv_buffer, 512, 0, 0, 0);

  if (recv < 0)
    goto error;
  else if (BDSM_DEBUG)
    printf("netbios_ns_resolve, received a reply for '%s' !\n", name);

  return (ntohl(*(uint32_t *)(recv_buffer + recv - 4)));

  error:
    perror("netbios_ns_resolve: ");
    return (0);
}


