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

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "smb_session_msg.h"
#include "smb_fd.h"
#include "smb_utils.h"
#include "smb_file.h"

smb_fd      smb_fopen(smb_session *s, smb_tid tid, const char *path,
                      uint32_t o_flags)
{
    smb_share       *share;
    smb_file        *file;
    smb_message     *req_msg, resp_msg;
    smb_create_req req;
    smb_create_resp *resp;
    size_t           path_len;
    int              res;
    char            *utf_path;

    assert(s != NULL && path != NULL);
    if ((share = smb_session_share_get(s, tid)) == NULL)
        return (0);

    path_len = smb_to_utf16(path, strlen(path) + 1, &utf_path);
    if (path_len == 0)
        return (0);

    req_msg = smb_message_new(SMB_CMD_CREATE);
    if (!req_msg) {
        free(utf_path);
        return (0);
    }

    // Set SMB Headers
    req_msg->packet->header.tid = tid;

    // Create AndX Params
    SMB_MSG_INIT_PKT_ANDX(req);
    req.wct            = 24;
    req.flags          = 0;
    req.root_fid       = 0;
    req.access_mask    = o_flags;
    req.alloc_size     = 0;
    req.file_attr      = 0;
    req.share_access   = SMB_SHARE_READ | SMB_SHARE_WRITE;
    req.disposition    = 1;  // 1 = Open and file if doesn't exist
    req.create_opts    = 0;  // We dont't support create
    req.impersonation  = 2;  // ?????
    req.security_flags = 0;  // ???
    req.path_length    = path_len;
    req.bct            = path_len + 1;
    SMB_MSG_PUT_PKT(req_msg, req);

    // Create AndX 'Body'
    smb_message_put8(req_msg, 0);   // Align beginning of path
    smb_message_append(req_msg, utf_path, path_len);
    free(utf_path);

    // smb_message_put16(req_msg, 0);  // ??

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
    if (!file)
        return (0);

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
    smb_close_req   req;

    assert(s != NULL);
    if (!fd)
      return;

    // XXX Memory leak, destroy the file after removing it
    if ((file = smb_session_file_remove(s, fd)) == NULL)
        return;

    msg = smb_message_new(SMB_CMD_CLOSE);
    if (!msg) {
        free(file->name);
        free(file);
        return;
    }

    msg->packet->header.tid = SMB_FD_TID(fd);

    SMB_MSG_INIT_PKT(req);
    req.wct        = 3;
    req.fid        = SMB_FD_FID(fd);
    req.last_write = ~0;
    req.bct        = 0;
    SMB_MSG_PUT_PKT(msg, req);

    // We don't check for succes or failure, since we actually don't really
    // care about creating a potentiel leak server side.
    smb_session_send_msg(s, msg);
    smb_session_recv_msg(s, 0);
    smb_message_destroy(msg);

    free(file->name);
    free(file);
}

ssize_t   smb_fread(smb_session *s, smb_fd fd, void *buf, size_t buf_size)
{
    smb_file        *file;
    smb_message     *req_msg, resp_msg;
    smb_read_req    req;
    smb_read_resp   *resp;
    size_t          max_read;
    int             res;

    assert(s != NULL && buf != NULL);
    if (!fd)
      return (-1);
    if ((file = smb_session_file_get(s, fd)) == NULL)
        return (-1);

    req_msg = smb_message_new(SMB_CMD_READ);
    if (!req_msg)
        return (-1);
    req_msg->packet->header.tid = file->tid;

    max_read = 0xffff;
    max_read = max_read < buf_size ? max_read : buf_size;

    SMB_MSG_INIT_PKT_ANDX(req);
    req.wct              = 12;
    req.fid              = file->fid;
    req.offset           = file->readp;
    req.max_count        = max_read;
    req.min_count        = max_read;
    req.max_count_high   = 0;
    req.remaining        = 0;
    req.offset_high      = 0;
    req.bct              = 0;
    SMB_MSG_PUT_PKT(req_msg, req);

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
