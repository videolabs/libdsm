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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if !defined( _WIN32 )
# include <arpa/inet.h>
#else
# include <winsock2.h>
#endif

#include "bdsm.h"

static void print_entry(const char *what, void *p_opaque,
                        netbios_ns_entry *entry)
{
  struct in_addr addr;

  addr.s_addr = netbios_ns_entry_ip(entry);

  printf("%s(%p): Ip: %s, name: %s/%s<%x>\n",
    what,
    p_opaque,
    inet_ntoa(addr),
    netbios_ns_entry_group(entry),
    netbios_ns_entry_name(entry),
    netbios_ns_entry_type(entry));
}

static void on_entry_added(void *p_opaque, netbios_ns_entry *entry)
{
  print_entry("added", p_opaque, entry);
}

static void on_entry_removed(void *p_opaque, netbios_ns_entry *entry)
{
  print_entry("removed", p_opaque, entry);
}

int main()
{
  netbios_ns *ns;
  netbios_ns_discover_callbacks callbacks;

  ns = netbios_ns_new();

  callbacks.p_opaque = (void*)0x42;
  callbacks.pf_on_entry_added = on_entry_added;
  callbacks.pf_on_entry_removed = on_entry_removed;

  printf("Discovering...\nPress Enter to quit\n");
  if (netbios_ns_discover_start(ns,
                                4, // broadcast every 4 seconds
                                &callbacks))
  {
    fprintf(stderr, "Error while discovering local network\n");
    exit(42);
  }

  getchar();

  netbios_ns_discover_stop(ns);

  return 0;
}
