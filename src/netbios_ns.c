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

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

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

  ns->entries       = NULL;
  ns->last_trn_id   = rand();

  return (ns);
}

void          netbios_ns_destroy(netbios_ns_t *ns)
{
  netbios_ns_entry_t  *next;

  if (!ns)
    return;

  netbios_ns_entry_clear(ns);

  close(ns->socket);
  free(ns);
}

int               netbios_ns_send_query(netbios_ns_t *ns, netbios_query_t *q,
                                        uint32_t ip)
{
  struct sockaddr_in  addr;
  ssize_t             sent;
  uint16_t            trn_id;

  assert(ns != NULL && q != NULL);

  trn_id = ns->last_trn_id + 1; // Increment transaction ID, not to reuse them
  q->packet->trn_id = htons(trn_id);

  addr.sin_addr.s_addr  = ip;
  addr.sin_family       = AF_INET;
  addr.sin_port         = htons(NETBIOS_PORT_NAME);

  sent = sendto(ns->socket, (void *)q->packet,
                sizeof(netbios_query_packet_t) + q->cursor, 0,
                (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  netbios_query_destroy(q);

  if (sent < 0){
    perror("netbios_ns_send_query: ");
    return (0);
  }

  ns->last_trn_id = trn_id; // Remember the last transaction id.
  return (1);
}

ssize_t           netbios_ns_recv(int sock, void *buf, size_t buf_size,
                                 struct timeval *timeout, struct sockaddr *addr,
                                 socklen_t *addr_len)
{
  fd_set        read_fds, error_fds;
  int           res;

  assert(sock && buf != NULL && buf_size > 0);

  FD_ZERO(&read_fds);
  FD_ZERO(&error_fds);
  FD_SET(sock, &read_fds);
  FD_SET(sock, &error_fds);

  res = select(sock + 1, &read_fds, 0, &error_fds, timeout);

  if (res < 0)
    goto error;
  if (FD_ISSET(sock, &error_fds))
    goto error;

  if (FD_ISSET(sock, &read_fds))
  {
    return (recvfrom(sock, buf, buf_size, 0, addr, addr_len));
  }
  else
    return (0);

  error:
    perror("netbios_ns_recv: ");
    return (-1);
}

uint32_t      netbios_ns_resolve(netbios_ns_t *ns, const char *name, char type)
{
  struct timeval      timeout;
  netbios_query_t     *q;
  char                *encoded_name;
  char                footer[4] = { 0x00, 0x20, 0x00, 0x01 };
  char                recv_buffer[512]; // Hu ?
  ssize_t             recv;
  uint16_t            trn_id;
  uint32_t            ip;


  assert(ns != NULL);

  if ((encoded_name = netbios_name_encode(name, 0, type)) == NULL)
    return (0);

    // Prepare packet
  q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
  netbios_query_set_flag(q, NETBIOS_FLAG_RECURSIVE, 1);
  netbios_query_set_flag(q, NETBIOS_FLAG_BROADCAST, 1);

  // Append the queried name to the packet
  netbios_query_append(q, encoded_name, strlen(encoded_name) + 1);

  // Magic footer (i.e. Question type (Netbios) / class (IP)
  netbios_query_append(q, footer, 4);
  q->packet->queries = htons(1);

  // We broadcast this query
  inet_aton("255.255.255.255", (struct in_addr *)&ip);

  // Let's send it
  if (!netbios_ns_send_query(ns, q, ip))
    return (0);
  else if (BDSM_DEBUG)
    fprintf(stderr, "netbios_ns_resolve, name query sent for '%s' !\n", name);

  free(encoded_name);

  // Now wait for a reply and pray
  timeout.tv_sec = 2;
  timeout.tv_usec = 420;
  recv = netbios_ns_recv(ns->socket, (void *)recv_buffer, 512, &timeout, 0, 0);

  if (recv <= 0)
    goto error;
  else if (BDSM_DEBUG)
    fprintf(stderr, "netbios_ns_resolve, received a reply for '%s' !\n", name);

  return (*(uint32_t *)(recv_buffer + recv - 4));

  error:
    perror("netbios_ns_resolve: ");
    return (0);
}

int           netbios_ns_discover(netbios_ns_t *ns)
{
  const char  broadcast_name[] = NETBIOS_WILDCARD;
  char        footer[4]        = { 0x00, 0x20, 0x00, 0x01 };

  struct sockaddr_in  recv_addr;
  socklen_t           recv_addr_len;
  struct timeval      timeout;

  netbios_query_t     *q;
  char                recv_buffer[512]; // Hu ?
  ssize_t             recv;
  uint32_t            ip;

  assert (ns != NULL);

  //
  // First step, we broadcast a packet to receive a message from every
  // NETBIOS nodes on the local network
  //
  q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
  // Append the queried name to the packet
  netbios_query_append(q, broadcast_name, strlen(broadcast_name) + 1);
  // Magic footer (i.e. Question type (Netbios) / class (IP)
  netbios_query_append(q, footer, 4);
  q->packet->queries = htons(1);

  // We broadcast this query
  inet_aton("255.255.255.255", (struct in_addr *)&ip);

  // Let's send it
  if (!netbios_ns_send_query(ns, q, ip))
  {
    fprintf(stderr, "Unable to send netbios 'discovery query'.\n");
    return (0);
  }
  else if (BDSM_DEBUG)
    fprintf(stderr, "netbios_ns_discover, name query sent for '*'.\n");



  //
  // Second step, we list every IP that answered to our broadcast.
  //

  netbios_ns_entry_clear(ns); // Let's reset our internal host list

  timeout.tv_sec = 1;
  timeout.tv_usec = 420;
  recv_addr_len = sizeof(recv_addr);
  while (netbios_ns_recv(ns->socket, (void *)recv_buffer, 512,
    &timeout, (struct sockaddr *)&recv_addr, &recv_addr_len) > 0)
  {
    // Verify this is a reply to our request
    if (ntohs(*(uint16_t *)recv_buffer) != ns->last_trn_id)
      fprintf(stderr, "Not a reply to our query: %u (our trn_id was: %u)\n",
        ntohs(*(uint16_t * )recv_buffer), ns->last_trn_id);
    else // Add the ip to the list of IP to check
    {
      netbios_ns_entry_add(ns, NULL, recv_addr.sin_addr.s_addr);
      if (BDSM_DEBUG)
        fprintf(stderr, "Discover: received a reply from %s\n",
                inet_ntoa(recv_addr.sin_addr));
    }
  }

  netbios_ns_entry_t *iter = ns->entries;
  while(iter != NULL)
  {
    netbios_ns_inverse(ns, iter);
    iter = iter->next;
  }

  return (1);
}

// Perform inverse name resolution. Grap an IP and return the first <20> field
// returned by the host
int           netbios_ns_inverse(netbios_ns_t *ns, netbios_ns_entry_t *entry)
{
  const char  broadcast_name[] = NETBIOS_WILDCARD;
  char        footer[4]        = { 0x00, 0x21, 0x00, 0x01 }; // NBSTAT/IP

  struct timeval      timeout;
  netbios_query_t     *q;
  char                recv_buffer[512]; // Hu ?
  ssize_t             recv;

  assert(ns != NULL && entry != NULL);

  // Prepare NBSTAT query packet
  q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
  netbios_query_append(q, broadcast_name, strlen(broadcast_name) + 1);
  netbios_query_append(q, footer, 4);
  q->packet->queries = htons(1);

  // Let's send it
  if (!netbios_ns_send_query(ns, q, entry->address.s_addr))
    return (0);
  else if (BDSM_DEBUG)
    fprintf(stderr, "netbios_ns_inverse, reverse name query sent for '%s' !\n",
            inet_ntoa(entry->address));

  // Now wait for a reply and pray
  timeout.tv_sec = 1;
  timeout.tv_usec = 500;
  recv = netbios_ns_recv(ns->socket, (void *)recv_buffer, 512, &timeout, 0, 0);

  if (recv <= 0)
    goto error;
  else if (BDSM_DEBUG)
    fprintf(stderr, "netbios_ns_inverse, received a reply for '%s' !\n",
            inet_ntoa(entry->address));


  // Now we've got something, let's find the <20> name
  netbios_query_packet_t  *p = (netbios_query_packet_t *)recv_buffer;
  uint8_t                 name_count;
  uint8_t                 name_idx;
  char                    *names;
  char                    *current_name;

  printf("Queried name length: %u\n", p->payload[0]);
  name_count = p->payload[p->payload[0] + 12];
  printf("Number of names: %hhu\n", name_count);
  names = p->payload + p->payload[0] + 13;

  for(name_idx = 0; name_idx < name_count; name_idx++)
  {
    current_name = names + name_idx * 18;
    if (current_name[15] == 0x20)
    {
      if (BDSM_DEBUG)
        fprintf(stderr, "Found name : %s\n", current_name);
      memcpy(entry->name, current_name, NETBIOS_NAME_LENGTH + 2);
      return (1);
    }
  }

  return(0);

  error:
    perror("netbios_ns_inverse: ");
    return (0);
}
