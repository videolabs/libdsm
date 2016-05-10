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

#include "config.h"

#include <assert.h>

#include "smb_fd.h"

void        smb_session_share_add(smb_session *s, smb_share *share)
{
    smb_share *iter;

    assert(s != NULL && share != NULL);

    if (s->shares == NULL)
    {
        s->shares = share;
        return;
    }

    iter = s->shares;
    while (iter->next != NULL)
        iter = iter->next;
    iter->next = share;
}

smb_share *smb_session_share_get(smb_session *s, smb_tid tid)
{
    smb_share *iter;

    assert(s != NULL);

    iter = s->shares;
    while (iter != NULL && iter->tid != tid)
        iter = iter->next;

    return iter;
}

smb_share *smb_session_share_remove(smb_session *s, smb_tid tid)
{
    smb_share *iter, *keep;

    assert(s != NULL);

    iter = s->shares;

    if (iter == NULL)
        return NULL;
    if (iter->tid == tid)
    {
        s->shares = s->shares->next;
        return iter;
    }

    while (iter->next != NULL && iter->next->tid != tid)
        iter = iter->next;

    if (iter->next != NULL) // We found it
    {
        keep = iter->next;
        iter->next = iter->next->next;
        return keep;
    }
    return NULL;
}

void            smb_session_share_clear(smb_session *s)
{
    smb_share   *iter, *tmp;
    smb_file    *fiter, *ftmp;

    assert(s != NULL);

    iter = s->shares;
    while(iter != NULL)
    {        
        fiter = iter->files;
        while(fiter != NULL)
        {
            ftmp = fiter;
            fiter = fiter->next;

            free(ftmp->name);
            free(ftmp);
        }   

        tmp = iter;
        iter = iter->next;
        free(tmp);
    }
}

int         smb_session_file_add(smb_session *s, smb_tid tid, smb_file *f)
{
    smb_share *share;
    smb_file  *iter;

    assert(s != NULL && f != NULL);

    if ((share = smb_session_share_get(s, tid)) == NULL)
        return 0;

    if (share->files == NULL)
        share->files = f;
    else
    {
        iter = share->files;
        while (iter->next != NULL)
            iter = iter->next;
        iter->next = f;
    }

    return 1;
}

smb_file  *smb_session_file_get(smb_session *s, smb_fd fd)
{
    smb_share *share;
    smb_file  *iter;

    assert(s != NULL && fd);

    if ((share = smb_session_share_get(s, SMB_FD_TID(fd))) == NULL)
        return NULL;

    iter = share->files;
    while (iter != NULL && iter->fid != SMB_FD_FID(fd))
        iter = iter->next;

    return iter;
}

smb_file  *smb_session_file_remove(smb_session *s, smb_fd fd)
{
    smb_share *share;
    smb_file  *iter, *keep;

    assert(s != NULL && fd);

    if ((share = smb_session_share_get(s, SMB_FD_TID(fd))) == NULL)
        return NULL;

    iter = share->files;

    if (iter == NULL)
        return NULL;
    if (iter->fid == SMB_FD_FID(fd))
    {
        share->files = iter->next;
        return iter;
    }

    while (iter->next != NULL && iter->next->fid != SMB_FD_TID(fd))
        iter = iter->next;
    if (iter->next != NULL)
    {
        keep = iter->next;
        iter->next = iter->next->next;
        return keep;
    }
    else
        return NULL;
}
