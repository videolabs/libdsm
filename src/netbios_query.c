// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>

#include "bdsm/netbios_query.h"

netbios_query_t   *netbios_query_new(size_t payload_size,
                                     int is_query, char opcode)
{
  netbios_query_t *q;

  q = malloc(sizeof(netbios_query_t));
  assert(q);
  memset((void *)q, 0, sizeof(netbios_query_t));

  q->packet = malloc(sizeof(netbios_query_packet_t) + payload_size);
  assert(q->packet);
  memset((void *)q->packet, 0, sizeof(netbios_query_packet_t) + payload_size);

  q->payload_size = payload_size;

  q->packet->flags  = htons(opcode << 11);
  netbios_query_set_flag(q, NETBIOS_FLAG_QUERY, !is_query);

  return (q);
}

void              netbios_query_destroy(netbios_query_t *q)
{
  assert(q);

  if (q->packet)
    free(q->packet);
  free(q);
}

void              netbios_query_set_flag(netbios_query_t *q,
                                         uint16_t flag, int value)
{
  assert(q && q->packet);

  if (value)
    q->packet->flags = htons(ntohs(q->packet->flags) | flag);
  else
    q->packet->flags = htons(ntohs(q->packet->flags) & ~flag);
}

void              netbios_query_print(netbios_query_t *q)
{
  assert(q && q->packet);

  printf("--- netbios_query_t dump :\n");
  printf("payload = %zu, cursor = %zu.\n", q->payload_size, q->cursor);
  printf("Transaction id = %u.\n", q->packet->trn_id);

  printf("-------------------------\n");
  for(unsigned i = 0; i < sizeof(netbios_query_packet_t) + q->cursor; i++)
  {
    char c;
    if ((i % 8) == 0 && i != 0)
      printf("\n");
    if ((i % 8) == 0)
      printf("0x");

    c = ((char *)q->packet)[i];
    printf("%.2hhX ", c);
  }
  printf("\n");
  printf("-------------------------\n");
}

int               netbios_query_append(netbios_query_t *q, const char *data,
                                       size_t data_size)
{
  assert(q && q->packet);

  if (q->payload_size - q->cursor < data_size)
    return (-1);

  memcpy(((char *)&q->packet->payload) + q->cursor, data, data_size);
  q->cursor += data_size;
  return (0);
}
