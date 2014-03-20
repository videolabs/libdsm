// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_NETBIOS_QUERY_H_
#define __BDSM_NETBIOS_QUERY_H_

#include <stdint.h>
#include <stddef.h>

#include "bdsm/netbios_defs.h"

netbios_query_t   *netbios_query_new(uint16_t trn_id, size_t payload_size,
                                     int is_query, char opcode);
void              netbios_query_destroy(netbios_query_t *q);
void              netbios_query_set_flag(netbios_query_t *q,
                                         uint16_t flag, int value);
int               netbios_query_append(netbios_query_t *q, const char *data,
                                       size_t data_size);

void              netbios_query_print(netbios_query_t *q);

#endif
