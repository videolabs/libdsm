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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "smb_session_msg.h"
#include "smb_fd.h"
#include "smb_utils.h"
#include "smb_file.h"
#include "bdsm_debug.h"

int         smb_fopen(smb_session *s, smb_tid tid, const char *path,
                      uint32_t o_flags, smb_fd *fd)
{
    smb_share       *share;
    smb_file        *file;
    smb_message     *req_msg, resp_msg;
    smb_create_req req;
    smb_create_resp *resp;
    size_t           path_len;
    int              res;
    char            *utf_path;

    assert(s != NULL && path != NULL && fd != NULL);

    if ((share = smb_session_share_get(s, tid)) == NULL)
        return DSM_ERROR_GENERIC;

    path_len = smb_to_utf16(path, strlen(path) + 1, &utf_path);
    if (path_len == 0)
        return DSM_ERROR_CHARSET;

    req_msg = smb_message_new(SMB_CMD_CREATE);
    if (!req_msg) {
        free(utf_path);
        return DSM_ERROR_GENERIC;
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
    if ((o_flags & SMB_MOD_RW) == SMB_MOD_RW)
    {
        req.disposition    = SMB_DISPOSITION_FILE_SUPERSEDE; // Create if doesn't exist
        req.create_opts    = SMB_CREATEOPT_WRITE_THROUGH;
    }
    else
    {
        req.disposition    = SMB_DISPOSITION_FILE_OPEN;  // Open and fails if doesn't exist
        req.create_opts    = 0;                          // We dont't support create
    }
    req.impersonation  = SMB_IMPERSONATION_SEC_IMPERSONATE;
    req.security_flags = SMB_SECURITY_NO_TRACKING;
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
        return DSM_ERROR_NETWORK;

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_NETWORK;
    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_create_resp))
    {
        BDSM_dbg("[smb_fopen]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_create_resp *)resp_msg.packet->payload;
    file = calloc(1, sizeof(smb_file));
    if (!file)
        return DSM_ERROR_GENERIC;


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

    *fd = SMB_FD(tid, file->fid);
    return DSM_SUCCESS;
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

    assert(s != NULL);

    if ((file = smb_session_file_get(s, fd)) == NULL)
        return -1;

    req_msg = smb_message_new(SMB_CMD_READ);
    if (!req_msg)
        return -1;
    req_msg->packet->header.tid = file->tid;

    max_read = 0xffff;
    max_read = max_read < buf_size ? max_read : buf_size;

    SMB_MSG_INIT_PKT_ANDX(req);
    req.wct              = 12;
    req.fid              = file->fid;
    req.offset           = file->offset;
    req.max_count        = max_read;
    req.min_count        = max_read;
    req.max_count_high   = 0;
    req.remaining        = 0;
    req.offset_high      = (file->offset >> 32) & 0xffffffff;
    req.bct              = 0;
    SMB_MSG_PUT_PKT(req_msg, req);

    res = smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);
    if (!res)
        return -1;

    if (!smb_session_recv_msg(s, &resp_msg))
        return -1;
    if (!smb_session_check_nt_status(s, &resp_msg))
        return -1;

    if (resp_msg.payload_size < sizeof(smb_read_resp))
    {
        BDSM_dbg("[smb_fread]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_read_resp *)resp_msg.packet->payload;

    if (resp_msg.packet->payload + resp_msg.payload_size <
        (uint8_t *)resp_msg.packet + resp->data_offset + resp->data_len)
    {
        BDSM_dbg("[smb_fread]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    if (buf)
        memcpy(buf, (char *)resp_msg.packet + resp->data_offset, resp->data_len);
    smb_fseek(s, fd, resp->data_len, SEEK_CUR);

    return resp->data_len;
}

ssize_t   smb_fwrite(smb_session *s, smb_fd fd, void *buf, size_t buf_size)
{
    smb_file       *file;
    smb_message    *req_msg, resp_msg;
    smb_write_req   req;
    smb_write_resp *resp;
    uint16_t        max_write;
    int             res;

    assert(s != NULL && buf != NULL);

    file = smb_session_file_get(s, fd);
    if (file == NULL)
        return -1;

    req_msg = smb_message_new(SMB_CMD_WRITE);
    if (!req_msg)
        return -1;
    req_msg->packet->header.tid = (uint16_t)file->tid;

    // total size of SMB message shall not exceed maximum size of netbios data payload
    max_write = UINT16_MAX - sizeof(smb_packet) - sizeof(smb_write_req);
    max_write = max_write < buf_size ? max_write : (uint16_t)buf_size;

    SMB_MSG_INIT_PKT_ANDX(req);
    req.wct              = 14; // Must be 14
    req.fid              = file->fid;
    req.offset           = file->offset & 0xffffffff;
    req.timeout          = 0;
    req.write_mode       = SMB_WRITEMODE_WRITETHROUGH;
    req.remaining        = 0;
    req.reserved         = 0;
    req.data_len         = max_write;
    req.data_offset      = sizeof(smb_packet) + sizeof(smb_write_req);
    req.offset_high      = (file->offset >> 32) & 0xffffffff;
    req.bct              = max_write;
    SMB_MSG_PUT_PKT(req_msg, req);
    smb_message_append(req_msg, buf, max_write);

    res = smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);
    if (!res)
        return -1;

    if (!smb_session_recv_msg(s, &resp_msg))
        return -1;
    if (!smb_session_check_nt_status(s, &resp_msg))
        return -1;

    if (resp_msg.payload_size < sizeof(smb_write_resp))
    {
        BDSM_dbg("[smb_fwrite]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_write_resp *)resp_msg.packet->payload;

    smb_fseek(s, fd, resp->data_len, SEEK_CUR);

    return resp->data_len;
}

ssize_t   smb_fseek(smb_session *s, smb_fd fd, off_t offset, int whence)
{
    smb_file  *file;

    assert(s != NULL);

    if ((file = smb_session_file_get(s, fd)) == NULL)
        return -1;

    if (whence == SMB_SEEK_SET)
        file->offset = offset;
    else if (whence == SMB_SEEK_CUR)
        file->offset += offset;

    return file->offset;
}

int  smb_file_rm(smb_session *s, smb_tid tid, const char *path)
{
    smb_message           *req_msg, resp_msg;
    smb_file_rm_req       req;
    smb_file_rm_resp      *resp;
    size_t                utf_pattern_len;
    char                  *utf_pattern;

    assert(s != NULL && path != NULL);

    utf_pattern_len = smb_to_utf16(path, strlen(path) + 1, &utf_pattern);
    if (utf_pattern_len == 0)
        return DSM_ERROR_CHARSET;

    req_msg = smb_message_new(SMB_CMD_RMFILE);
    if (!req_msg)
    {
        free(utf_pattern);
        return DSM_ERROR_GENERIC;
    }

    req_msg->packet->header.tid = (uint16_t)tid;
    req_msg->packet->header.flags2 = SMB_FLAGS2_LONG_NAMES;

    SMB_MSG_INIT_PKT(req);
    req.wct               = 0x01; // Must be 1
    req.search_attributes = SMB_ATTR_HIDDEN | SMB_ATTR_SYS;
    req.bct               = (uint16_t)(utf_pattern_len + 1);
    req.buffer_format     = 0x04; // Must be 4
    SMB_MSG_PUT_PKT(req_msg, req);
    smb_message_append(req_msg, utf_pattern, utf_pattern_len);

    smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);

    free(utf_pattern);

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_NETWORK;
    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_file_rm_resp))
    {
        BDSM_dbg("[smb_file_rm]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_file_rm_resp *)resp_msg.packet->payload;
    if ((resp->wct != 0) || (resp->bct != 0))
        return DSM_ERROR_NETWORK;

    return 0;
}

int       smb_file_mv(smb_session *s, smb_tid tid, const char *old_path, const char *new_path)
{
    smb_message           *req_msg, resp_msg;
    smb_file_mv_req       req;
    smb_file_mv_resp      *resp;
    size_t                utf_old_len,utf_new_len;
    char                  *utf_old_path,*utf_new_path;

    assert(s != NULL && old_path != NULL && new_path != NULL);

    utf_old_len = smb_to_utf16(old_path, strlen(old_path) + 1, &utf_old_path);
    if (utf_old_len == 0)
        return DSM_ERROR_CHARSET;

    utf_new_len = smb_to_utf16(new_path, strlen(new_path) + 1, &utf_new_path);
    if (utf_new_len == 0)
    {
        free(utf_old_path);
        return DSM_ERROR_CHARSET;
    }

    req_msg = smb_message_new(SMB_CMD_MOVE);
    if (!req_msg)
    {
        free(utf_old_path);
        free(utf_new_path);
        return DSM_ERROR_GENERIC;
    }

    req_msg->packet->header.tid = (uint16_t)tid;
    req_msg->packet->header.flags2 = SMB_FLAGS2_LONG_NAMES;

    SMB_MSG_INIT_PKT(req);
    req.wct               = 0x01; // Must be 1
    req.search_attributes = SMB_ATTR_HIDDEN | SMB_ATTR_SYS;
    req.bct               = (uint16_t)(utf_old_len + utf_new_len + 3); // We have 2 bytes for buffer formats and 1 padding byte
    SMB_MSG_PUT_PKT(req_msg, req);
    smb_message_put8(req_msg, 0x04); // Buffer format 1, must be 4
    smb_message_append(req_msg, utf_old_path, utf_old_len);
    smb_message_put8(req_msg, 0x04); // Buffer format 2, must be 4
    smb_message_put8(req_msg, 0x00); // padding to have next byte 16 bits aligned
    smb_message_append(req_msg, utf_new_path, utf_new_len);

    smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);

    free(utf_old_path);
    free(utf_new_path);

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_NETWORK;

    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_file_mv_resp))
    {
        BDSM_dbg("[smb_file_mv]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_file_mv_resp *)resp_msg.packet->payload;
    if ((resp->wct != 0) || (resp->bct != 0))
        return DSM_ERROR_NETWORK;

    return DSM_SUCCESS;
}
