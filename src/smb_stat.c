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

#include <assert.h>

#include "bdsm/smb_stat.h"
#include "bdsm/smb_fd.h"

smb_stat        smb_stat_fd(smb_session *s, smb_fd fd)
{
    assert(s != NULL && fd);

    return (smb_session_file_get(s, fd));
}

void            smb_stat_destroy(smb_stat stat) 
{
    smb_stat_list_destroy((smb_stat_list) stat); 
}

size_t            smb_stat_list_count(smb_stat_list list)
{
    size_t          count = 0;

    while (list != NULL)
    {
        list = list->next;
        ++count;
    }

    return (count);
}

void            smb_stat_list_destroy(smb_stat_list list)
{
    smb_stat_list tmp;

    while(list != NULL)
    {
        tmp = list->next;
        free(list->name);
        free(list);
        list = tmp;
    }
}

smb_stat        smb_stat_list_at(smb_stat_list list, size_t index)
{
    size_t          pos = 0;

    while (list != NULL && pos < index)
    {
        list = list->next;
        pos++;
    }

    return (list);
}

const char        *smb_stat_name(smb_stat info)
{
    if (info == NULL)
        return (NULL);
    else
        return (info->name);
}

uint64_t          smb_stat_get(smb_stat info, int what)
{
    if (info == NULL)
        return (0);

    switch (what)
    {
        case SMB_STAT_SIZE:
            return (info->size);
        case SMB_STAT_ALLOC_SIZE:
            return (info->alloc_size);
        case SMB_STAT_CTIME:
            return (info->created);
        case SMB_STAT_ATIME:
            return (info->accessed);
        case SMB_STAT_WTIME:
            return (info->written);
        case SMB_STAT_MTIME:
            return (info->changed);
        case SMB_STAT_ISDIR:
            return (info->is_dir);
        default:
            return (0);
    }
}
