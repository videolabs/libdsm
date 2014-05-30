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

#ifndef __BDSM_NETBIOS_QUERY_H_
#define __BDSM_NETBIOS_QUERY_H_

#include <stdint.h>
#include <stddef.h>

#include "bdsm/netbios_defs.h"

typedef struct              netbios_query_s
{
    size_t                      payload_size;
    size_t                      cursor;
    netbios_query_packet      *packet;
}                           netbios_query;

netbios_query   *netbios_query_new(size_t payload_size, int is_query,
                                   char opcode);
void              netbios_query_destroy(netbios_query *q);
void              netbios_query_set_flag(netbios_query *q,
        uint16_t flag, int value);
int               netbios_query_append(netbios_query *q, const char *data,
                                       size_t data_size);

void              netbios_query_print(netbios_query *q);


#endif
