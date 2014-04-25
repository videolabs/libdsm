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

#ifndef __BDSM_NETBIOS_NS_H_
#define __BDSM_NETBIOS_NS_H_

#include "bdsm/netbios_defs.h"
#include "bdsm/netbios_query.h"

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

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
typedef struct                netbios_ns_entry_s
{
  struct netbios_ns_entry_s     *next;
  struct in_addr                address;
  char                          name[NETBIOS_NAME_LENGTH + 1];
  char                          type;
}                             netbios_ns_entry_t;

/**
 * @brief Get the name of the entry referenced by the iterator iter.
 * @details The pointer points to an area of memory owned by the netbios name
 * service
 *
 * @return A null-terminated ASCII string representing the name of a netbios machine.
 */
const char          *netbios_ns_entry_name(netbios_ns_entry_t *entry);

/**
 * @brief Return the IP address of the correspondance referenced by the iterator
 *
 * @return The ip address of this entry, in network byte order.
 */
uint32_t            netbios_ns_entry_ip(netbios_ns_entry_t *entry);

/**
 * @brief Return the type of record
 *
 * @return The type of netbios record (.ie 0x20 for FileServer,
 * 0 for workstation, etc.) or a value < 0 if the iterator is invalid or an
 * error occured.
 */
char                netbios_ns_entry_type(netbios_ns_entry_t *entry);

/**
 * @brief The netbios name service object.
 *
 * @details Holds all the necessary data structure to perform resolution and
 * discovery, and to stores the results. Consider it as an opaque data
 * structure, use the related functions to interact with it.
 */

typedef struct {
  int                 socket;
  struct sockaddr_in  addr;
  uint16_t            last_trn_id;  // Last transaction id used;
  netbios_ns_entry_t  *entries;     // NS entries cache, mainly used by discover()
}                   netbios_ns_t;

/**
 * @brief Allocate and initialize the Netbios name service client object.
 * @return A newly allocated netbios_ns_t ready for querying.
 * Deallocate with netbios_ns_destroy().
 */
netbios_ns_t  *netbios_ns_new();

/**
 * @brief Destroy the netbios name service object
 * @param[in] ns A pointer on the netbios_ns_t to destroy and deallocate
 */
void          netbios_ns_destroy(netbios_ns_t *ns);

/**
 * @brief Resolve a Netbios name
 * @details This function tries to resolves the given NetBIOS name with the
 * given type on the LAN, using broadcast queries. No WINS server is called.
 *
 * @param ns the netbios name service object.
 * @param name the null-terminated ASCII netbios name to resolve. If it's
 * longer than 15 chars, it'll be truncated.
 * @param type The type of the name to look for. @see netbios_defs.h
 * @param[out] addr The IP address in network byte order of the machine if found.
 * @return the ipv4 address in network byte-order or 0 if it wasn't successfull.
 */
int             netbios_ns_resolve(netbios_ns_t *ns, const char *name,
                                   char type, uint32_t * addr);

/**
 * @brief Try to discover all the Netbios/SMB speaking machine on the LAN.
 * @details This functions sends a message to '*' Netbios name, and waits for
 * the machine on the LAN to answer. It then performs a reverse lookup on all
 * the ip he received packet from. It stores the results inside of the name
 * service, allowing you to list them
 *
 *
 * @param ns The name service object.
 * @return It returns 0 in case of error.
 */
int           netbios_ns_discover(netbios_ns_t *ns);

/**
 * @brief Get the list of entries (know machine) for this name service object
 * @details You might want to call discover before-hand if you don't want
 * the lit to be empty
 *
 * @return The list of entries in the name service.
 */
int             netbios_ns_entry_count(netbios_ns_t *ns);

/**
 * @brief Get the entry at a certain position in the entry list
 * @details You might want to call discover before-hand if you don't want
 * the lit to be empty. The entry list contains all the record known to the
 * name service (including resolved, reverse resolved and discovered) since the
 * creation of the name service object or the last call to clear
 *
 * @param ns The nameservice object.
 * @param pos The index/position of the item to access in the list. Must be <
 * netbios_ns_entry_count(ns) or the pointer returned will be NULL.
 * @return A pointer to a opaque netbios_ns_entry_t structure
 */
netbios_ns_entry_t *netbios_ns_entry_at(netbios_ns_t *ns, int pos);

/**
 * @brief Perform an inverse netbios lookup (get name from ip)
 * @details This function does a NBSTAT and stores all the returned entry in
 * the internal list of entries. It returns one of the name found. (Normally
 * the <20> or <0> name)
 *
 * @param ns The name service object.
 * @param ip The ip address in network byte order.
 *
 * @return A null-terminated ASCII string containing the NETBIOS name. You don't
 * own the it (it'll be freed when destroying/clearing the name service)
 */
const char          *netbios_ns_inverse(netbios_ns_t *ns, uint32_t ip);

/**
 * @brief Clear all the existing entries from the name service
 *
 * @param ns The nameservice object
 */
void                netbios_ns_clear(netbios_ns_t *ns);

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
netbios_ns_entry_t  *netbios_ns_entry_add(netbios_ns_t *ns, const char *name,
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
netbios_ns_entry_t  *netbios_ns_entry_find(netbios_ns_t *ns, const char *by_name,
                                           uint32_t ip);

#endif
