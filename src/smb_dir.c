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
 * Author: Sylver Bruneau <sylver.bruneau@gmail.com>
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
#include "smb_dir.h"

uint32_t  smb_directory_rm(smb_session *s, smb_tid tid, const char *path)
{
    smb_message           *req_msg, resp_msg;
    smb_directory_rm_req  req;
    smb_directory_rm_resp *resp;
    size_t                utf_pattern_len;
    char                  *utf_pattern;

    if (s == NULL)
        return DSM_ERROR_INVALID_SESSION;
    if (tid == -1)
        return DSM_ERROR_INVALID_TID;
    if (path == NULL)
        return DSM_ERROR_INVALID_PATH;

    utf_pattern_len = smb_to_utf16(path, strlen(path) + 1, &utf_pattern);
    if (utf_pattern_len == 0)
        return DSM_ERROR_UTF16_CONV_FAILED;

    req_msg = smb_message_new(SMB_CMD_RMDIR);
    if (!req_msg)
    {
        free(utf_pattern);
        return DSM_ERROR_INTERNAL;
    }

    req_msg->packet->header.tid = (uint16_t)tid;

    SMB_MSG_INIT_PKT(req);
    req.wct              = 0x00; // Must be 0
    req.bct              = (uint16_t)(utf_pattern_len + 1);
    req.buffer_format    = 0x04; // Must be 4
    SMB_MSG_PUT_PKT(req_msg, req);
    smb_message_append(req_msg, utf_pattern, utf_pattern_len);

    smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);

    free(utf_pattern);

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_INVALID_RCV_MESS;

    if (resp_msg.packet->header.status != NT_STATUS_SUCCESS)
        return resp_msg.packet->header.status;

    resp = (smb_directory_rm_resp *)resp_msg.packet->payload;
    if ((resp->wct != 0) || (resp->bct != 0))
        return DSM_ERROR_INVALID_RCV_MESS;

    return NT_STATUS_SUCCESS;
}

uint32_t  smb_directory_create(smb_session *s, smb_tid tid, const char *path)
{
    smb_message              *req_msg, resp_msg;
    smb_trans2_req           tr2;
    smb_tr2_create_directory create_dir;
    size_t                   utf_path_len, msg_len;
    char                     *utf_path;
    int                      padding = 0;

    if (s == NULL)
        return DSM_ERROR_INVALID_SESSION;
    if (tid == -1)
        return DSM_ERROR_INVALID_TID;
    if (path == NULL)
        return DSM_ERROR_INVALID_PATH;

    utf_path_len = smb_to_utf16(path, strlen(path) + 1, &utf_path);
    if (utf_path_len == 0)
        return DSM_ERROR_UTF16_CONV_FAILED;

    msg_len   = sizeof(smb_trans2_req) + sizeof(smb_tr2_query);
    msg_len  += utf_path_len;
    if (msg_len %4)
        padding = 4 - msg_len % 4;

    req_msg = smb_message_new(SMB_CMD_TRANS2);
    if (!req_msg) {
        free(utf_path);
        return DSM_ERROR_INTERNAL;
    }
    req_msg->packet->header.tid = (uint16_t)tid;

    SMB_MSG_INIT_PKT(tr2);
    tr2.wct                = 15;
    tr2.total_param_count  = (uint16_t)(utf_path_len + sizeof(smb_tr2_create_directory));
    tr2.param_count        = tr2.total_param_count;
    tr2.max_param_count    = 2; // ?? Why not the same or 12 ?
    tr2.max_data_count     = 0xffff;
    tr2.param_offset       = 68; // Offset of find_first_params in packet;
    tr2.data_count         = 0;
    tr2.data_offset        = 96; // Offset of pattern in packet
    tr2.setup_count        = 1;
    tr2.cmd                = SMB_TR2_CREATE_DIRECTORY;
    tr2.bct                = (uint16_t)(sizeof(smb_tr2_create_directory) + utf_path_len + padding);
    SMB_MSG_PUT_PKT(req_msg, tr2);

    SMB_MSG_INIT_PKT(create_dir);
    create_dir.reserved = 0x00000000; // Must be 0
    SMB_MSG_PUT_PKT(req_msg, create_dir);

    smb_message_append(req_msg, utf_path, utf_path_len);
    free(utf_path);

    // Adds padding at the end if necessary.
    while (padding--)
        smb_message_put8(req_msg, 0);

    smb_session_send_msg(s, req_msg);
    smb_message_destroy(req_msg);

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_INVALID_RCV_MESS;

    return resp_msg.packet->header.status;
}
