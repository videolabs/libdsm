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

#ifdef _WIN32
#define EAFNOSUPPORT WSAEAFNOSUPPORT
int inet_pton (int af, const char *src, void *dst)
{
    unsigned char *b = dst;
    switch (af)
    {
        case AF_INET:
            return sscanf (src, "%hhu.%hhu.%hhu.%hhu",
                    b + 0, b + 1, b + 2, b + 3) == 4;
    }
    errno = EAFNOSUPPORT;
    return -1;
}
#endif

int main(int ac, char **av)
{
  netbios_ns        *ns;
  struct in_addr      addr;
  const char          *name;

  ns = netbios_ns_new();

  if (ac != 2)
  {
    fprintf(stderr, "%s usage: %s a.b.c.d\n", av[0], av[0]);
    fprintf(stderr, "Print the netbios name for this IP address\n");
    exit(1);
  }

  inet_pton(AF_INET, av[1], &addr);
  if ((name = netbios_ns_inverse(ns, addr.s_addr)) == NULL)
  {
    fprintf(stderr, "Unable to perform inverse name resolution for %s\n", av[1]);
    exit(42);
  }

  printf("%s\n", name);
  netbios_ns_destroy(ns);
  return 0;
}
