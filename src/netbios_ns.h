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

#ifndef _NETBIOS_NS_H_
#define _NETBIOS_NS_H_

#include <bdsm/netbios_ns.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "netbios_defs.h"
#include "netbios_query.h"

/**
 * @file netbios_ns.h
 * @brief Netbios name service
 */

/**
 * @brief Represents an correspondance between an IP address and a Netbios name.
 *
 * @details Consider it as an opaque data structure whose internal layout might
 * change at any time, please use the provided accessors functions
 */
struct netbios_ns_entry
{
    netbios_ns_entry              *next;
    struct in_addr                address;
    char                          name[NETBIOS_NAME_LENGTH + 1];
    char                          type;
};

struct netbios_ns
{
    int                 socket;
    struct sockaddr_in  addr;
    uint16_t            last_trn_id;  // Last transaction id used;
    netbios_ns_entry    *entries;     // NS entries cache, mainly used by discover()
};

/**
 * @internal
 * @brief Add an entry to the name service list.
 * @details You can provide a name and/or an ip
 *
 * @param ns The name service object.
 * @param name The ASCII name of the entry or NULL
 * @param type the <X> type for this entry or -1
 * @param ip The IP address in network byte order (or 0)
 * @return The added entry
 */
netbios_ns_entry    *netbios_ns_entry_add(netbios_ns *ns, const char *name,
        char type, uint32_t ip);
/**
 * @internal
 * @brief Find an entry in
 * @details [long description]
 *
 * @param ns [description]
 * @param by_name [description]
 * @param ip [description]
 * @return [description]
 */
netbios_ns_entry    *netbios_ns_entry_find(netbios_ns *ns, const char *by_name,
        uint32_t ip);

#endif
