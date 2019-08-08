/*****************************************************************************
 *  __________________    _________  _____            _____  .__         ._.
 *  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
 *   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
 *   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
 *   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
 *          \/        \/        \/        \/  )/           \/        \/   \/
 *
 * This file is part of liBDSM. Copyright © 2014-2015 VideoLabs SAS
 *
 * Author: Julien 'Lta' BALLET <contact@lta.io>
 *
 * liBDSM is released under LGPLv2.1 (or later) and is also available
 * under a commercial license.
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef __BDSM_NETBIOS_NS_H_
#define __BDSM_NETBIOS_NS_H_

#include <stdint.h>

/**
 * @file netbios_ns.h
 * @brief Netbios name service
 */

typedef struct netbios_ns_entry netbios_ns_entry;
typedef struct netbios_ns netbios_ns;

/**
 * @brief Get the name of the entry referenced by the iterator iter.
 * @details The pointer points to an area of memory owned by the netbios name
 * service
 *
 * @return A null-terminated ASCII string representing the name of a netbios machine.
 */
const char          *netbios_ns_entry_name(netbios_ns_entry *entry);

/**
 * @brief Get the name of the entry referenced by the iterator iter.
 * @details The pointer points to an area of memory owned by the netbios name
 * service
 *
 * @return A null-terminated ASCII string representing the group of a netbios machine.
 */
const char          *netbios_ns_entry_group(netbios_ns_entry *entry);

/**
 * @brief Return the IP address of the correspondance referenced by the iterator
 *
 * @return The ip address of this entry, in network byte order.
 */
uint32_t            netbios_ns_entry_ip(netbios_ns_entry *entry);

/**
 * @brief Return the type of record
 *
 * @return The type of netbios record (.ie 0x20 for FileServer,
 * 0 for workstation, etc.) or a value < 0 if the iterator is invalid or an
 * error occured.
 */
char                netbios_ns_entry_type(netbios_ns_entry *entry);

/**
 * @brief Allocate and initialize the Netbios name service client object.
 * @return A newly allocated netbios_ns ready for querying.
 * Deallocate with netbios_ns_destroy().
 */
netbios_ns    *netbios_ns_new();

/**
 * @brief Destroy the netbios name service object
 * @param[in] ns A pointer on the netbios_ns to destroy and deallocate
 */
void          netbios_ns_destroy(netbios_ns *ns);

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
 * @return 0 on success or -1 on failure
 */
int           netbios_ns_resolve(netbios_ns *ns, const char *name,
                                 char type, uint32_t *addr);

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
const char          *netbios_ns_inverse(netbios_ns *ns, uint32_t ip);

typedef struct
{
    // Opaque pointer that will be passed to callbacks
    void *p_opaque;
    // Called when a new entry is added
    void (*pf_on_entry_added) (void *p_opaque, netbios_ns_entry *entry);
    // Called when an entry is removed
    void (*pf_on_entry_removed)(void *p_opaque, netbios_ns_entry *entry);
} netbios_ns_discover_callbacks;

/** @brief Perform a NETBIOS discovery in a separate thread.
 *
 * @details This functions starts a new thread that will send a message to '*'
 * Netbios name (broadcast NB query on 255.255.255.255). It'll wait for the
 * machine on the LAN to answer. It'll then perform a reverse lookup on all the
 * ip he received packet from. Once a name and an ip is found, this function
 * will notify the caller by a callback.
 *
 * @param ns The name service object.  @param broadcast_timeout Do a broadcast
 * every timeout seconds @param callbacks The callbacks previously setup by the
 * caller
 *
 * @return 0 on success or -1 on failure
 */
int netbios_ns_discover_start(netbios_ns *ns, unsigned int broadcast_timeout,
                              netbios_ns_discover_callbacks *callbacks);

/**
 * @brief Stop the NETBIOS discovery.
 * @param ns The name service object.
 * @return 0 on success or -1 on failure
 */
int netbios_ns_discover_stop(netbios_ns *ns);

#endif
