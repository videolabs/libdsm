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

#include "bdsm_debug.h"
#include "smb_session_msg.h"
#include "smb_utils.h"
#include "smb_fd.h"
#include "smb_share.h"
#include "smb_file.h"

int smb_tree_connect(smb_session *s, const char *name, smb_tid *tid)
{
    smb_tree_connect_req  req;
    smb_tree_connect_resp *resp;
    smb_message            resp_msg;
    smb_message           *req_msg;
    smb_share             *share;
    size_t                 path_len, utf_path_len;
    char                  *path, *utf_path;

    assert(s != NULL && name != NULL && tid != NULL);

    req_msg = smb_message_new(SMB_CMD_TREE_CONNECT);
    if (!req_msg)
        return DSM_ERROR_GENERIC;

    // Build \\SERVER\Share path from name
    path_len  = strlen(name) + strlen(s->srv.name) + 4;
    path      = alloca(path_len);
    snprintf(path, path_len, "\\\\%s\\%s", s->srv.name, name);
    utf_path_len = smb_to_utf16(path, strlen(path) + 1, &utf_path);

    // Packet headers
    req_msg->packet->header.tid   = 0xffff; // Behavior of libsmbclient

    smb_message_set_andx_members(req_msg);

    // Packet payload
    SMB_MSG_INIT_PKT_ANDX(req);
    req.wct          = 4;
    req.flags        = 0x0c; // (??)
    req.passwd_len   = 1;    // Null byte
    req.bct = utf_path_len + 6 + 1;
    SMB_MSG_PUT_PKT(req_msg, req);

    smb_message_put8(req_msg, 0); // Ze null byte password;
    smb_message_append(req_msg, utf_path, utf_path_len);
    free(utf_path);
    smb_message_append(req_msg, "?????", strlen("?????") + 1);

    if (!smb_session_send_msg(s, req_msg))
    {
        smb_message_destroy(req_msg);
        return DSM_ERROR_NETWORK;
    }
    smb_message_destroy(req_msg);

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_NETWORK;

    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_tree_connect_resp))
    {
        BDSM_dbg("[smb_tree_connect]Malformed message\n");
        return DSM_ERROR_NETWORK;
    }

    resp  = (smb_tree_connect_resp *)resp_msg.packet->payload;
    share = calloc(1, sizeof(smb_share));
    if (!share)
        return DSM_ERROR_GENERIC;

    share->tid          = resp_msg.packet->header.tid;
    share->opts         = resp->opt_support;
    share->rights       = resp->max_rights;
    share->guest_rights = resp->guest_rights;

    smb_session_share_add(s, share);

    *tid = share->tid;
    return 0;
}

int           smb_tree_disconnect(smb_session *s, smb_tid tid)
{
    smb_tree_disconnect_req   req;
    smb_tree_disconnect_resp *resp;
    smb_message              *req_msg;
    smb_message               resp_msg;

    assert(s != NULL);

    req_msg = smb_message_new(SMB_CMD_TREE_DISCONNECT);
    if (!req_msg)
        return DSM_ERROR_GENERIC;

    // Packet headers
    req_msg->packet->header.tid = (uint16_t)tid;

    // Packet payload
    req.wct = 0; // Must be 0
    req.bct = 0; // Must be 0
    SMB_MSG_PUT_PKT(req_msg, req);

    if (!smb_session_send_msg(s, req_msg))
    {
        smb_message_destroy(req_msg);
        return DSM_ERROR_NETWORK;
    }
    smb_message_destroy(req_msg);

    if (!smb_session_recv_msg(s, &resp_msg))
        return DSM_ERROR_NETWORK;
    if (!smb_session_check_nt_status(s, &resp_msg))
        return DSM_ERROR_NT;

    if (resp_msg.payload_size < sizeof(smb_tree_disconnect_resp))
    {
        BDSM_dbg("[smb_tree_disconnect]Malformed message\n");
        return DSM_ERROR_NETWORK;
    }

    resp  = (smb_tree_disconnect_resp *)resp_msg.packet->payload;
    if ((resp->wct != 0) || (resp->bct != 0))
        return DSM_ERROR_NETWORK;

    return DSM_SUCCESS;
}

// Here we parse the NetShareEnumAll response packet payload to extract
// The share list.
static ssize_t  smb_share_parse_enum(smb_message *msg, char ***list)
{
    uint32_t          share_count, i;
    uint8_t           *data, *eod;
    uint32_t          *p_share_count;

    assert(msg != NULL && list != NULL);
    // Let's skip smb parameters and DCE/RPC stuff until we are at the begginning of
    // NetShareCtrl

    p_share_count = (uint32_t *)(msg->packet->payload + 60);
    share_count = *p_share_count;
    data        = msg->packet->payload + 72 + share_count * 12;
    eod         = msg->packet->payload + msg->payload_size;

    *list       = calloc(share_count + 1, sizeof(char *));
    if (!(*list))
        return -1;

    for (i = 0; i < share_count && data < eod; i++)
    {
        uint32_t name_len, com_len;

        if (data + sizeof(uint32_t) > eod)
        {
            BDSM_dbg("[smb_share_parse_enum]Malformed message\n");
            break;
        }

        name_len = *((uint32_t *)data);   // Read 'Max Count', make it a multiple of 2
        data    += 3 * sizeof(uint32_t);  // Move pointer to beginning of Name.

        if (data +  sizeof(uint32_t) + name_len * 2 + sizeof(uint32_t) > eod)
        {
            BDSM_dbg("[smb_share_parse_enum]Malformed message\n");
            break;
        }

        smb_from_utf16((const char *)data, name_len * 2, (*list) + i);

        if (name_len % 2) name_len += 1;  // Align next move
        data    += name_len * 2;          // Move the pointer to Comment 'Max count'

        com_len  = *((uint32_t *)data);
        data    += 3 * sizeof(uint32_t);  // Move pointer to beginning of Comment.
        if (com_len % 2) com_len += 1;    // Align next move
        data    += com_len * 2;           // Move the pointer to next item
    }

    return i;
}

size_t          smb_share_list_count(smb_share_list list)
{
    size_t        res;

    if (list == NULL)
        return 0;

    for (res = 0; list[res] != NULL; res++)
        ;

    return res;
}

const char      *smb_share_list_at(smb_share_list list, size_t index)
{
    assert(list != NULL);

    return list[index];
}

void            smb_share_list_destroy(smb_share_list list)
{
    assert(list != NULL);

    for (size_t i = 0; list[i] != NULL; i++)
        free(list[i]);
    free(list);
}

// We should normally implement SCERPC and SRVSVC to perform a share list. But
// since these two protocols have no other use for us, we'll do it the trash way
// PS: Worst function _EVER_. I don't understand a bit myself
int             smb_share_get_list(smb_session *s, smb_share_list *list, size_t *pcount)
{
    smb_message           *req, resp;
    smb_trans_req         trans;
    smb_tid               ipc_tid;
    smb_fd                srvscv_fd;
    uint16_t              rpc_len;
    size_t                res, frag_len_cursor;
    ssize_t               count;
    int                   ret;

    assert(s != NULL && list != NULL);
    *list = NULL;

    if ((ret = smb_tree_connect(s, "IPC$", &ipc_tid)) != DSM_SUCCESS)
        return ret;

    if ((ret = smb_fopen(s, ipc_tid, "\\srvsvc", SMB_MOD_READ | SMB_MOD_WRITE,
                         &srvscv_fd)) != DSM_SUCCESS)
        return ret;

    //// Phase 1:
    // We bind a context or whatever for DCE/RPC

    req = smb_message_new(SMD_CMD_TRANS);
    if (!req)
    {
        ret = DSM_ERROR_GENERIC;
        goto error;
    }
    req->packet->header.tid = ipc_tid;

    rpc_len = 0xffff;
    SMB_MSG_INIT_PKT(trans);
    trans.wct                    = 16;
    trans.total_data_count       = 72;
    trans.max_data_count         = rpc_len;
    trans.param_offset           = 84;
    trans.data_count             = 72;
    trans.data_offset            = 84;
    trans.setup_count            = 2;
    trans.pipe_function          = 0x26;
    trans.fid                    = SMB_FD_FID(srvscv_fd);
    trans.bct                    = 89;
    SMB_MSG_PUT_PKT(req, trans);

    smb_message_put8(req, 0);   // Padding
    smb_message_put_utf16(req, "\\PIPE\\", strlen("\\PIPE\\") + 1);
    smb_message_put16(req, 0);  // Padding to be aligned with wtf boundary :-/

    // Now we'll 'build' the DCE/RPC Packet. This basically a copycat
    // from wireshark values.
    smb_message_put8(req, 5);     // version major
    smb_message_put8(req, 0);     // minor
    smb_message_put8(req, 0x0b);  // Packet type = 'bind'
    smb_message_put8(req, 0x03);  // Packet flags = ??
    smb_message_put32(req, 0x10); // Representation = little endian/ASCII. Damn
    smb_message_put16(req, 72);   // Data len again
    smb_message_put16(req, 0);    // Auth len ?
    smb_message_put32(req, 19);   // Call ID ?
    smb_message_put16(req, rpc_len);      // Max Xmit size
    smb_message_put16(req, rpc_len);      // Max Recv size
    smb_message_put32(req, 0);    // Assoc group ?

    smb_message_put32(req, 1);    // Num Ctx Item
    // Adding the CtxItem, whatever could that be
    smb_message_put16(req, 0);    // ContextID
    smb_message_put16(req, 1);    // Num Trans Item
    // SRVSVC UUID
    const uint8_t uuid_e[8] = {0x12, 0x78, 0x5a, 0x47, 0xbf, 0x6e, 0xe1, 0x88};
    smb_message_put_uuid(req, 0x4b324fc8, 0x1670, 0x01d3, uuid_e);
    smb_message_put16(req, 3);    // Version
    smb_message_put16(req, 0);    // Minor
    // Another UUID
    const uint8_t uuid_e2[8] = {0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60};
    smb_message_put_uuid(req, 0x8a885d04, 0x1ceb, 0x11c9, uuid_e2);
    smb_message_put32(req, 2);    // Another version

    // Let's send this ugly pile of shit over the network !
    res = smb_session_send_msg(s, req);
    smb_message_destroy(req);
    if (!res)
    {
        ret = DSM_ERROR_NETWORK;
        goto error;
    }

    // Is the server throwing pile of shit back at me ?
    res = smb_session_recv_msg(s, &resp);
    if (resp.payload_size < 71)
    {
        BDSM_dbg("[smb_share_get_list]Malformed message\n");
        ret = DSM_ERROR_NETWORK;
        goto error;
    }

    if (!res || resp.packet->payload[68])
    {
        BDSM_dbg("Bind call failed: 0x%hhx (reason = 0x%hhx)\n",
                 resp.packet->payload[68], resp.packet->payload[70]);
        ret = DSM_ERROR_NETWORK;
        goto error;
    }


    //// Phase 2:
    // Now we have the 'bind' done (regarless of what it is), we'll call
    // NetShareEnumAll

    req = smb_message_new(SMD_CMD_TRANS);
    if (!req)
    {
        ret = DSM_ERROR_GENERIC;
        goto error;
    }
    req->packet->header.tid = ipc_tid;

    // this struct will be set at the end when we know the data size
    SMB_MSG_ADVANCE_PKT(req, smb_trans_req);

    smb_message_put8(req, 0);  // Padding
    smb_message_put_utf16(req, "\\PIPE\\", strlen("\\PIPE\\") + 1);
    smb_message_put16(req, 0); // Padding

    // Now we'll 'build' the DCE/RPC Packet. This basically a copycat
    // from wireshark values.
    smb_message_put8(req, 5);     // version major
    smb_message_put8(req, 0);     // minor
    smb_message_put8(req, 0);     // Packet type = 'request'
    smb_message_put8(req, 0x03);  // Packet flags = ??
    smb_message_put32(req, 0x10); // Representation = little endian/ASCII. Damn
    // Let's save the cursor here to update that later
    frag_len_cursor = req->cursor;
    smb_message_put16(req, 0);    // Data len again (frag length)
    smb_message_put16(req, 0);    // Auth len ?
    smb_message_put32(req, 12);   // Call ID ?
    smb_message_put32(req, 64);   // Alloc hint ?
    smb_message_put16(req, 0);    // Context ID ?
    smb_message_put16(req, 15);   // OpNum = NetShareEnumAll

    // Pointer to server UNC
    smb_message_put32(req, 0x00020000);   // Referent ID ?
    smb_message_put32(req, strlen(s->srv.name) + 1);            // Max count
    smb_message_put32(req, 0);            // Offset
    smb_message_put32(req, strlen(s->srv.name) + 1);            // Actual count
    // The server name, supposed to be downcased
    smb_message_put_utf16(req, s->srv.name, strlen(s->srv.name) + 1);
    if ((strlen(s->srv.name) % 2) == 0) // It won't be aligned with the terminating byte
        smb_message_put16(req, 0);

    smb_message_put32(req, 1);            // Level 1 ?
    smb_message_put32(req, 1);            // Ctr ?
    smb_message_put32(req, 0x00020004);   // Referent ID ?
    smb_message_put64(req, 0);            // Count/Null Pointer to NetShareInfo1
    smb_message_put32(req, 0xffffffff);   // Max Buffer (0xffffffff required by smbX)

    smb_message_put32(req, 0x00020008);   // Referent ID ?
    smb_message_put32(req, 0);            // Resume ?

    // fill trans pkt at the end since we know the size at the end
    SMB_MSG_INIT_PKT(trans);
    trans.wct              = 16;
    trans.max_data_count   = 4280;
    trans.setup_count      = 2;
    trans.pipe_function    = 0x26; // TransactNmPipe;
    trans.fid              = SMB_FD_FID(srvscv_fd);
    trans.bct              = req->cursor - sizeof(smb_trans_req);
    trans.data_count       = trans.bct - 17; // 17 -> padding + \PIPE\ + padding
    trans.total_data_count = trans.data_count;
    trans.data_offset      = 84;
    trans.param_offset     = 84;
    // but insert it at the begining
    SMB_MSG_INSERT_PKT(req, 0, trans);

    req->packet->payload[frag_len_cursor] = trans.data_count; // (data_count SHOULD stay < 256)

    // Let's send this ugly pile of shit over the network !
    res = smb_session_send_msg(s, req);
    smb_message_destroy(req);
    if (!res)
    {
        ret = DSM_ERROR_NETWORK;
        goto error;
    }

    // Is the server throwing pile of shit back at me ?
    res = smb_session_recv_msg(s, &resp);
    if (resp.payload_size < 4)
    {
        BDSM_dbg("[smb_share_get_list]Malformed message\n");
        ret = DSM_ERROR_NETWORK;
        goto error;
    }

    if (!res && (uint32_t)resp.packet->payload[resp.payload_size - 4])
    {
        BDSM_dbg("NetShareEnumAll call failed.\n");
        ret = DSM_ERROR_NETWORK;
        goto error;
    }


    //// Phase 3
    // We parse the list of Share (finally !) and build function response
    count = smb_share_parse_enum(&resp, list);
    if (count == -1)
    {
        ret = DSM_ERROR_GENERIC;
        goto error;
    }
    if (pcount != NULL)
        *pcount = count;
    ret = DSM_SUCCESS;

error:
    // Close the pipe
    smb_fclose(s, srvscv_fd);
    return ret;
}
