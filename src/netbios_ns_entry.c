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
#include <assert.h>

#include "bdsm/netbios_ns.h"

netbios_ns_iter_t   netbios_ns_iter_next(netbios_ns_iter_t iter)
{
  if (iter != NULL)
    return (iter->next);
  else
    return (NULL);
}

const char          *netbios_ns_iter_name(netbios_ns_iter_t iter)
{
  if (iter != NULL)
    return (iter->name);
  else
    return (NULL);
}

uint32_t            netbios_ns_iter_ip(netbios_ns_iter_t iter)
{
  if (iter != NULL)
    return (iter->address.s_addr);
  else
    return (0);
}

char                netbios_ns_iter_type(netbios_ns_iter_t iter)
{
  if (iter != NULL)
    return (iter->type);
  else
    return (-1);
}

void                netbios_ns_clear(netbios_ns_t *ns)
{
  netbios_ns_entry_t  *next;

  assert(ns != NULL);

  while (ns->entries != NULL)
  {
    next = ns->entries->next;
    free(ns->entries->next);
    ns->entries = next;
  }
}

netbios_ns_entry_t *netbios_ns_entry_add(netbios_ns_t *ns, const char *name,
                                         char type, uint32_t ip)
{
  netbios_ns_entry_t  *entry;

  entry = malloc(sizeof(netbios_ns_entry_t));
  assert(entry != NULL);
  memset((void *)entry, 0, sizeof(netbios_ns_entry_t));

  if (name != NULL)
  {
    memcpy(entry->name, name, NETBIOS_NAME_LENGTH);
    entry->name[NETBIOS_NAME_LENGTH] = 0;
  }

  entry->type           = type;
  entry->address.s_addr = ip;
  entry->next           = ns->entries;

  ns->entries = entry;

  return (ns->entries);
}
// Find an entry in the list. Search by name if name is not NULL,
// or by ip otherwise
netbios_ns_entry_t *netbios_ns_entry_find(netbios_ns_t *ns, const char *by_name,
                                          uint32_t ip)
{
  netbios_ns_entry_t  *found = NULL;
  netbios_ns_entry_t  *iter;

  assert(ns != NULL);

  iter = ns->entries;
  while(iter != NULL && found == NULL)
  {
    if (by_name != NULL)
    {
      if (!strncmp(by_name, iter->name, NETBIOS_NAME_LENGTH + 1))
        found = iter;
    }
    else if (iter->address.s_addr == ip)
      found = iter;
  }

  return (found);
}
