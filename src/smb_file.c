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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "bdsm/smb_session_msg.h"
#include "bdsm/smb_fd.h"
#include "bdsm/smb_utils.h"
#include "bdsm/smb_file.h"

smb_fd      smb_fopen(smb_session *s, smb_tid tid, const char *path,
                      uint32_t o_flags)
{
    smb_share       *share;
    smb_file        *file;
    smb_message     *req_msg, resp_msg;
    smb_create_req  *req;
    smb_create_resp *resp;
    size_t           path_len;
    int              res;

    assert(s != NULL && path != NULL);
    if ((share = smb_session_share_get(s, tid)) == NULL)
        return (0);

    req_msg = smb_message_new(SMB_CMD_CREATE, 128);

    // Set SMB Headers
    smb_message_set_andx_members(req_msg);
    req_msg->packet->header.tid = tid;

    // Create AndX Params
    req = (smb_create_req *)req_msg->packet->payload;
    req->wct            = 24;
    req->flags          = 0;
    req->root_fid       = 0;
    req->access_mask    = o_flags;
    req->alloc_size     = 0;
    req->file_attr      = 0;
    req->share_access   = SMB_SHARE_READ | SMB_SHARE_WRITE;
    req->disposition    = 1;  // 1 = Open and file if doesn't exist
    req->create_opts    = 0;  // We dont't support create
    req->impersonation  = 2;  // ?????
    req->security_flags = 0;  // ???

    // Create AndX 'Body'
    smb_message_advance(req_msg, sizeof(smb_create_req));
    smb_message_put8(req_msg, 0);   // Align beginning of path
    path_len = smb_message_put_utf16(req_msg, path, strlen(path) + 1);
    // smb_message_put16(req_msg, 0);  // ??
    req->path_length  = path_len;
    req->bct          = path_len + 1;

    res = smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);
    if (!res)
        return (0);

    if (!smb_session_recv_msg(s, &resp_msg))
        return (0);
    if (resp_msg.packet->header.status != NT_STATUS_SUCCESS)
        return (0);

    resp = (smb_create_resp *)resp_msg.packet->payload;
    file = calloc(1, sizeof(smb_file));
    assert(file != NULL);

    file->fid           = resp->fid;
    file->tid           = tid;
    file->created       = resp->created;
    file->accessed      = resp->accessed;
    file->written       = resp->written;
    file->changed       = resp->changed;
    file->alloc_size    = resp->alloc_size;
    file->size          = resp->size;
    file->attr          = resp->attr;
    file->is_dir        = resp->is_dir;

    smb_session_file_add(s, tid, file); // XXX Check return

    return (SMB_FD(tid, file->fid));
}

void        smb_fclose(smb_session *s, smb_fd fd)
{
    smb_file        *file;
    smb_message     *msg;
    smb_close_req   *req;

    assert(s != NULL);
    if (!fd)
      return;

    // XXX Memory leak, destroy the file after removing it
    if ((file = smb_session_file_remove(s, fd)) == NULL)
        return;

    msg = smb_message_new(SMB_CMD_CLOSE, 64);
    req = (smb_close_req *)msg->packet->payload;

    msg->packet->header.tid = SMB_FD_TID(fd);

    smb_message_advance(msg, sizeof(smb_close_req));
    req->wct        = 3;
    req->fid        = SMB_FD_FID(fd);
    req->last_write = ~0;
    req->bct        = 0;

    // We don't check for succes or failure, since we actually don't really
    // care about creating a potentiel leak server side.
    smb_session_send_msg(s, msg);
    smb_session_recv_msg(s, 0);
}

ssize_t   smb_fread(smb_session *s, smb_fd fd, void *buf, size_t buf_size)
{
    smb_file        *file;
    smb_message     *req_msg, resp_msg;
    smb_read_req    *req;
    smb_read_resp   *resp;
    size_t          max_read;
    int             res;

    assert(s != NULL && buf != NULL);
    if (!fd)
      return (-1);
    if ((file = smb_session_file_get(s, fd)) == NULL)
        return (-1);

    req_msg = smb_message_new(SMB_CMD_READ, 64);
    req_msg->packet->header.tid = file->tid;
    smb_message_set_andx_members(req_msg);
    smb_message_advance(req_msg, sizeof(smb_read_req));

    max_read = 0xffff;
    max_read = max_read < buf_size ? max_read : buf_size;

    req = (smb_read_req *)req_msg->packet->payload;
    req->wct              = 12;
    req->fid              = file->fid;
    req->offset           = file->readp;
    req->max_count        = max_read;
    req->min_count        = max_read;
    req->max_count_high   = 0;
    req->remaining        = 0;
    req->offset_high      = 0;
    req->bct              = 0;

    res = smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);
    if (!res)
        return (-1);

    if (!smb_session_recv_msg(s, &resp_msg))
        return (-1);
    if (resp_msg.packet->header.status != NT_STATUS_SUCCESS)
        return (-1);

    resp = (smb_read_resp *)resp_msg.packet->payload;
    memcpy(buf, resp->file, resp->data_len);
    smb_fseek(s, fd, resp->data_len, SEEK_CUR);

    return (resp->data_len);
}

ssize_t   smb_fseek(smb_session *s, smb_fd fd, ssize_t offset, int whence)
{
    smb_file  *file;

    assert(s != NULL);

    if (!fd || (file = smb_session_file_get(s, fd)) == NULL)
        return (0);

    if (whence == SMB_SEEK_SET)
        file->readp = offset;
    else if (whence == SMB_SEEK_CUR)
        file->readp += offset;

    return (file->readp);
}
