// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bdsm/netbios_ns.h"

void                netbios_ns_entry_clear(netbios_ns_t *ns)
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
                                         uint32_t ip)
{
  netbios_ns_entry_t  *entry;

  entry = malloc(sizeof(netbios_ns_entry_t));
  assert(entry != NULL);
  memset((void *)entry, 0, sizeof(netbios_ns_entry_t));

  if (name != NULL)
    memcpy(entry->name, name, NETBIOS_NAME_LENGTH + 2);

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
