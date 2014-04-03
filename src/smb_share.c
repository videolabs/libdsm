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

#include "bdsm/smb_utils.h"
#include "bdsm/smb_share.h"
#include "bdsm/smb_file.h"


smb_tid         smb_tree_connect(smb_session_t *s, const char *name)
{
  smb_tree_connect_req_t  *req;
  smb_tree_connect_resp_t *resp;
  smb_message_t   resp_msg;
  smb_message_t   *req_msg;
  smb_share_t     *share;
  size_t          path_len;
  char            *path;

  assert(s != NULL && name != NULL);

  // Build \\SERVER\Share path from name
  path_len  = strlen(name) + strlen(s->srv.name) + 4;
  path      = alloca(path_len);
  snprintf(path, path_len, "\\\\%s\\%s", s->srv.name, name);

  req_msg = smb_message_new(SMB_CMD_TREE_CONNECT, 128);

  // Packet headers
  smb_message_set_default_flags(req_msg);
  smb_message_set_andx_members(req_msg);
  req_msg->packet->header.tid   = 0xffff; // Behavior of libsmbclient

  // Packet payload
  req = (smb_tree_connect_req_t *)req_msg->packet->payload;
  smb_message_advance(req_msg, sizeof(smb_tree_connect_req_t));
  req->wct          = 4;
  req->flags        = 0x0c; // (??)
  req->passwd_len   = 1;    // Null byte

  smb_message_put8(req_msg, 0); // Ze null byte password;
  smb_message_put_utf16(req_msg, "", path, strlen(path) + 1);
  smb_message_append(req_msg, "?????", strlen("?????") + 1);
  req->bct = strlen(path) * 2 + 2 + 6 + 1;

  if (!smb_session_send_msg(s, req_msg))
  {
    smb_message_destroy(req_msg);
    return (0);
  }
  smb_message_destroy(req_msg);

  if (!smb_session_recv_msg(s, &resp_msg))
    return (0);
  if (resp_msg.packet->header.status != NT_STATUS_SUCCESS)
    return (0);

  resp  = (smb_tree_connect_resp_t *)resp_msg.packet->payload;
  share = calloc(1, sizeof(smb_share_t));
  assert(share != NULL);

  share->tid          = resp_msg.packet->header.tid;
  share->opts         = resp->opt_support;
  share->rights       = resp->max_rights;
  share->guest_rights = resp->guest_rights;

  smb_session_share_add(s, share);

  return(share->tid);
}

int           smb_tree_disconnect(smb_session_t *s, smb_tid tid)
{
  assert(s != NULL && tid);
  fprintf(stderr, "smb_tree_disconnect: NOT IMPLEMENTED YET\n");
  return (0);
}

// We should normally implement SCERPC and SRVSVC to perform a share list. But
// since these two protocols have no other use for use, we'll do it the trash
// way
// PS: Worst function _EVER_. I don't understand a bit myself
size_t          smb_share_list(smb_session_t *s, smb_share_list_t **list)
{
  smb_message_t         *req, resp;
  smb_trans_req_t       *trans;
  smb_tid               ipc_tid;
  smb_fd                srvscv_fd;
  uint16_t              rpc_len;
  ssize_t               res;

  assert(s != NULL && list != NULL);
  *list = NULL;

  ipc_tid = smb_tree_connect(s, "IPC$");
  if (!ipc_tid)
    return (0);

  srvscv_fd = smb_fopen(s, ipc_tid, "\\srvsvc", SMB_MOD_RW);
  if (!srvscv_fd)
    return (0);

  //// Phase 1:
  // We bind a context or whatever for DCE/RPC

  req = smb_message_new(SMD_CMD_TRANS, 256);
  smb_message_set_default_flags(req);
  req->packet->header.tid = ipc_tid;

  smb_message_advance(req, sizeof(smb_trans_req_t));
  trans = (smb_trans_req_t *)req->packet->payload;

  memset((void *)trans, 0, sizeof(smb_trans_req_t));

  rpc_len = NETBIOS_SESSION_PAYLOAD - 256; // XXX

  trans->wct                    = 16;
  trans->total_data_count       = 72;
  trans->max_data_count         = rpc_len;
  trans->param_offset           = 84;
  trans->data_count             = 72;
  trans->data_offset            = 84;
  trans->setup_count            = 2;
  trans->pipe_function          = 0x26;
  trans->fid                    = SMB_FD_FID(srvscv_fd);
  trans->bct                    = 89;

  smb_message_put_utf16(req, "", "\\PIPE\\", strlen("\\PIPE\\") + 1);
  smb_message_put16(req, 0); // Padding to be aligned with wtf boundary :-/

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
    return (0);

  // Is the server throwing pile of shit back at me ?
  res = smb_session_recv_msg(s, &resp);
  if (!res || resp.packet->payload[68])
  {
    if (BDSM_DEBUG)
      fprintf(stderr, "Bind call failed: 0x%hhx (reason = 0x%hhx)\n",
              resp.packet->payload[68], resp.packet->payload[70]);
    return (0);
  }


  //// Phase 2:
  // Now we have the 'bind' (regarless of what it is), we'll call
  // NetShareEnumAll

  req = smb_message_new(SMD_CMD_TRANS, 256);
  smb_message_set_default_flags(req);
  req->packet->header.tid = ipc_tid;

  smb_message_advance(req, sizeof(smb_trans_req_t));
  trans = (smb_trans_req_t *)req->packet->payload;

  memset((void *)trans, 0, sizeof(smb_trans_req_t));


  return (1);
}
