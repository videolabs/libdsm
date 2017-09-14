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
#include "smb_dir.h"
#include "bdsm_debug.h"

int smb_directory_rm(smb_session *s, smb_tid tid, const char *path)
{
    smb_message           *req_msg, resp_msg;
    smb_directory_rm_req  req;
    smb_directory_rm_resp *resp;
    size_t                utf_pattern_len;
    char                  *utf_pattern;

    assert(s != NULL && path != NULL);

    utf_pattern_len = smb_to_utf16(path, strlen(path) + 1, &utf_pattern);
    if (utf_pattern_len == 0)
        return DSM_ERROR_CHARSET;

    req_msg = smb_message_new(SMB_CMD_RMDIR);
    if (!req_msg)
    {
        free(utf_pattern);
        return DSM_ERROR_GENERIC;
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
        return DSM_ERROR_NETWORK;

    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_directory_rm_resp))
    {
        BDSM_dbg("[smb_directory_rm]Malformed message.\n");
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_directory_rm_resp *)resp_msg.packet->payload;
    if ((resp->wct != 0) || (resp->bct != 0))
        return DSM_ERROR_NETWORK;

    return DSM_SUCCESS;
}

int smb_directory_create(smb_session *s, smb_tid tid, const char *path)
{
    smb_message           *req_msg, resp_msg;
    smb_directory_mk_req  req;
    smb_directory_mk_resp *resp;
    size_t                utf_pattern_len;
    char                  *utf_pattern;

    assert(s != NULL && path != NULL);

    utf_pattern_len = smb_to_utf16(path, strlen(path) + 1, &utf_pattern);
    if (utf_pattern_len == 0)
        return DSM_ERROR_CHARSET;

    req_msg = smb_message_new(SMB_CMD_MKDIR);
    if (!req_msg)
    {
        free(utf_pattern);
        return DSM_ERROR_GENERIC;
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
        return DSM_ERROR_NETWORK;

    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_directory_mk_resp))
    {
        BDSM_dbg("[smb_directory_create]Malformed message %s\n", path);
        return DSM_ERROR_NETWORK;
    }

    resp = (smb_directory_mk_resp *)resp_msg.packet->payload;
    if ((resp->wct != 0) || (resp->bct != 0))
        return DSM_ERROR_NETWORK;

    return DSM_SUCCESS;
}
