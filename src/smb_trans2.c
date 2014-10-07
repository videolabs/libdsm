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
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "bdsm/debug.h"
#include "bdsm/smb_message.h"
#include "bdsm/smb_session_msg.h"
#include "bdsm/smb_utils.h"
#include "bdsm/smb_stat.h"

static smb_file *smb_find_parse(smb_message *msg)
{
    smb_trans2_resp       *tr2;
    smb_tr2_find2_params  *params;
    smb_tr2_find2_entry   *iter;
    smb_file              *files, *tmp;
    uint8_t               *eod;
    size_t                count, i;

    assert(msg != NULL);

    // Let's parse the answer we got from server
    tr2     = (smb_trans2_resp *)msg->packet->payload;
    params  = (smb_tr2_find2_params *)tr2->payload;
    iter    = (smb_tr2_find2_entry *)(tr2->payload + sizeof(smb_tr2_find2_params));
    eod     = msg->packet->payload + msg->payload_size;
    count   = params->count;
    files   = NULL;

    for (i = 0; i < count && (uint8_t *)iter < eod; i++)
    {
        // Create a smb_file and fill it
        tmp = calloc(1, sizeof(smb_file));
        assert(tmp != NULL);

        tmp->name_len = smb_from_utf16((const char *)iter->name, iter->name_len,
                                       &tmp->name);
        tmp->name[tmp->name_len] = 0;

        tmp->created    = iter->created;
        tmp->accessed   = iter->accessed;
        tmp->written    = iter->written;
        tmp->changed    = iter->changed;
        tmp->size       = iter->size;
        tmp->alloc_size = iter->alloc_size;
        tmp->attr       = iter->attr;
        tmp->is_dir     = tmp->attr & SMB_ATTR_DIR;

        tmp->next = files;
        files     = tmp;

        iter = (smb_tr2_find2_entry *)(((char *)iter) + iter->next_entry);
    }

    return (files);
}

static smb_message *smb_tr2_recv(smb_session *s)
{
    smb_message           recv, *res;
    smb_trans2_resp       *tr2;
    size_t                growth;
    int                   remaining;

    if (!smb_session_recv_msg(s, &recv))
        return (NULL);

    tr2         = (smb_trans2_resp *)recv.packet->payload;
    growth      = tr2->total_data_count - tr2->data_count;
    res         = smb_message_grow(&recv, growth);
    res->cursor = recv.payload_size;
    remaining   = (int)tr2->total_data_count -
                  (tr2->data_displacement + tr2->data_count);

    while (remaining > 0)
    {
        remaining = smb_session_recv_msg(s, &recv);
        if (remaining)
        {
            tr2   = (smb_trans2_resp *)recv.packet->payload;
            /*
             * XXX: Why does padding was necessary and is not anymore, i.e.
             * find a reproductible setup where it is
             */
            smb_message_append(res, tr2->payload /* + 2 pad */, tr2->data_count);
            remaining = (int)tr2->total_data_count -
                        (tr2->data_displacement + tr2->data_count);
        }
    }

    return (res);
}

smb_file  *smb_find(smb_session *s, smb_tid tid, const char *pattern)
{
    smb_message           *msg;
    smb_trans2_req        *tr2;
    smb_tr2_find2         *find;
    size_t                pattern_len, msg_len, utf_pattern_len;
    int                   res;

    assert(s != NULL && pattern != NULL && tid);

    pattern_len = strlen(pattern) + 1;
    msg_len     = sizeof(smb_trans2_req) + sizeof(smb_tr2_find2);
    msg_len    += pattern_len * 2 + 3;

    msg = smb_message_new(SMB_CMD_TRANS2, msg_len);
    msg->packet->header.tid = tid;

    tr2 = (smb_trans2_req *)msg->packet->payload;
    tr2->wct                = 15;

    tr2->max_param_count    = 10; // ?? Why not the same or 12 ?
    tr2->max_data_count     = 0xffff;;
    tr2->param_offset       = 68; // Offset of find_first_params in packet;
    tr2->data_count         = 0;
    tr2->data_offset        = 88; // Offset of pattern in packet
    tr2->setup_count        = 1;
    tr2->cmd                = SMB_TR2_FIND_FIRST;

    find = (smb_tr2_find2 *) tr2->payload;
    find->attrs     = SMB_FIND2_ATTR_DEFAULT;
    find->count     = 1366;     // ??
    find->flags     = SMB_FIND2_FLAG_DEFAULT;
    find->interest  = 0x0104;   // 'Find file both directory info'

    smb_message_advance(msg, sizeof(smb_trans2_req));
    smb_message_advance(msg, sizeof(smb_tr2_find2));
    utf_pattern_len = smb_message_put_utf16(msg, pattern, pattern_len);

    tr2->bct = sizeof(smb_tr2_find2) + utf_pattern_len;
    tr2->total_param_count = tr2->bct;
    tr2->param_count       = tr2->bct;
    tr2->bct += 3; //3 == padding

    // Adds padding at the end if necessary.
    while ((tr2->bct % 4) != 3)
    {
        smb_message_put8(msg, 0);
        tr2->bct++;
    }

    res = smb_session_send_msg(s, msg);
    smb_message_destroy(msg);
    if (!res)
    {
        BDSM_dbg("Unable to query pattern: %s\n", pattern);
        return (NULL);
    }

    if ((msg = smb_tr2_recv(s)) == NULL)
        return (NULL);

    return (smb_find_parse(msg));
}


smb_file  *smb_fstat(smb_session *s, smb_tid tid, const char *path)
{
    smb_message           *msg, reply;
    smb_trans2_req        *tr2;
    smb_trans2_resp       *tr2_resp;
    smb_tr2_query         *query;
    smb_tr2_path_info     *info;
    smb_file              *file;
    size_t                path_len, msg_len;
    int                   res;

    assert(s != NULL && path != NULL && tid);

    path_len  = strlen(path) + 1;
    msg_len   = sizeof(smb_trans2_req) + sizeof(smb_tr2_query);
    msg_len  += path_len * 2 + 3; // +3 for eventual padding

    msg = smb_message_new(SMB_CMD_TRANS2, msg_len);
    msg->packet->header.tid = tid;

    tr2 = (smb_trans2_req *)msg->packet->payload;
    tr2->wct                = 15;
    tr2->total_param_count  = path_len * 2 + sizeof(smb_tr2_query);
    tr2->max_param_count    = 2; // ?? Why not the same or 12 ?
    tr2->max_data_count     = 0xffff;
    tr2->param_count        = tr2->total_param_count;
    tr2->param_offset       = 68; // Offset of find_first_params in packet;
    tr2->data_count         = 0;
    tr2->data_offset        = 96; // Offset of pattern in packet
    tr2->setup_count        = 1;
    tr2->cmd                = SMB_TR2_QUERY_PATH;
    tr2->bct                = sizeof(smb_tr2_query) + path_len * 2 + 3;

    query = (smb_tr2_query *)tr2->payload;
    query->interest   = 0x0107;   // Query File All Info

    smb_message_advance(msg, sizeof(smb_trans2_req));
    smb_message_advance(msg, sizeof(smb_tr2_query));
    smb_message_put_utf16(msg, path, path_len);

    // Adds padding at the end if necessary.
    if (msg->cursor % 4)
    {
        int padding = 4 - msg->cursor % 4;
        tr2->bct += padding;
        for (int i = 0; i < padding; i++)
            smb_message_put8(msg, 0);
    }

    res = smb_session_send_msg(s, msg);
    smb_message_destroy(msg);
    if (!res)
    {
        BDSM_dbg("Unable to query pattern: %s\n", path);
        return (NULL);
    }

    if (!smb_session_recv_msg(s, &reply)
        || reply.packet->header.status != NT_STATUS_SUCCESS)
    {
        BDSM_dbg("Unable to recv msg or failure for %s\n", path);
        return (NULL);
    }

    tr2_resp  = (smb_trans2_resp *)reply.packet->payload;
    info      = (smb_tr2_path_info *)(tr2_resp->payload + 4); //+4 is padding
    file      = calloc(1, sizeof(smb_file));
    assert(file != NULL);

    file->name_len  = smb_from_utf16((const char *)info->name, info->name_len,
                                     &file->name);
    file->name[info->name_len] = 0;

    file->created     = info->created;
    file->accessed    = info->accessed;
    file->written     = info->written;
    file->changed     = info->changed;
    file->alloc_size  = info->alloc_size;
    file->size        = info->size;
    file->attr        = info->attr;
    file->is_dir      = info->is_dir;

    return (file);
}
