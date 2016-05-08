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

int main(int ac, char **av)
{
  netbios_ns        *ns;
  struct in_addr      addr;

  ns = netbios_ns_new();

  if (ac != 2)
  {
    fprintf(stderr, "%s usage: %s NBT_NAME\n", av[0], av[0]);
    fprintf(stderr, "Print the ip for this netbios name\n");
    exit(1);
  }

  if (netbios_ns_resolve(ns, av[1], NETBIOS_FILESERVER, &addr.s_addr)) {
    exit(-1);
  }
  if (!addr.s_addr)
  {
    fprintf(stderr, "Unable to perform name resolution for %s\n", av[1]);
    exit(42);
  }

  printf("%s's IP address is : %s\n", av[1], inet_ntoa(addr));
  netbios_ns_destroy(ns);
  return 0;
}
