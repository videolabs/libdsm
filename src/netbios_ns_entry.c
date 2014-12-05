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

#include "netbios_ns.h"

const char          *netbios_ns_entry_name(netbios_ns_entry *entry)
{
    if (entry != NULL)
        return (entry->name);
    else
        return (NULL);
}

uint32_t            netbios_ns_entry_ip(netbios_ns_entry *entry)
{
    if (entry != NULL)
        return (entry->address.s_addr);
    else
        return (0);
}

char                netbios_ns_entry_type(netbios_ns_entry *entry)
{
    if (entry != NULL)
        return (entry->type);
    else
        return (-1);
}

void                netbios_ns_clear(netbios_ns *ns)
{
    netbios_ns_entry  *next;

    assert(ns != NULL);

    while (ns->entries != NULL)
    {
        next = ns->entries->next;
        free(ns->entries);
        ns->entries = next;
    }
}

netbios_ns_entry *netbios_ns_entry_add(netbios_ns *ns, const char *name,
                                       char type, uint32_t ip)
{
    netbios_ns_entry  *entry;

    entry = malloc(sizeof(netbios_ns_entry));
    assert(entry != NULL);
    memset((void *)entry, 0, sizeof(netbios_ns_entry));

    if (name != NULL)
    {
        memcpy(entry->name, name, NETBIOS_NAME_LENGTH);
        entry->name[NETBIOS_NAME_LENGTH] = 0;

        for (int i = 1; i < NETBIOS_NAME_LENGTH; i++ )
          if (entry->name[NETBIOS_NAME_LENGTH - i] == ' ')
            entry->name[NETBIOS_NAME_LENGTH - i] = 0;
          else
            break;
    }

    entry->type           = type;
    entry->address.s_addr = ip;
    entry->next           = ns->entries;

    ns->entries = entry;

    return (ns->entries);
}
// Find an entry in the list. Search by name if name is not NULL,
// or by ip otherwise
netbios_ns_entry *netbios_ns_entry_find(netbios_ns *ns, const char *by_name,
                                        uint32_t ip)
{
    netbios_ns_entry  *found = NULL;
    netbios_ns_entry  *iter;

    assert(ns != NULL);

    iter = ns->entries;
    while (iter != NULL && found == NULL)
    {
        if (by_name != NULL)
        {
            if (!strncmp(by_name, iter->name, NETBIOS_NAME_LENGTH))
                found = iter;
        }
        else if (iter->address.s_addr == ip)
            found = iter;

        iter = iter->next;
    }

    return (found);
}

int             netbios_ns_entry_count(netbios_ns *ns)
{
    netbios_ns_entry  *iter;
    int                 res;

    assert(ns != NULL);

    iter  = ns->entries;
    res   = 0;
    while (iter != NULL)
    {
        res++;
        iter = iter->next;
    }

    return (res);
}

netbios_ns_entry  *netbios_ns_entry_at(netbios_ns *ns, int pos)
{
    netbios_ns_entry  *iter = NULL;
    int                 i = 0;

    assert(ns != NULL);

    iter = ns->entries;
    while (i < pos && iter != NULL)
    {
        i++;
        iter = iter->next;
    }

    return (iter);
}
