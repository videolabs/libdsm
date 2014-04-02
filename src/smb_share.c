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

void        smb_session_share_add(smb_session_t *s, smb_share_t *share)
{
  smb_share_t *iter;

  assert(s != NULL && share != NULL);

  if (s->shares == NULL)
  {
    s->shares = share;
    return;
  }

  iter = s->shares;
  while(iter->next != NULL)
    iter = iter->next;
  iter->next = share;
}

smb_share_t *smb_session_share_get(smb_session_t *s, smb_tid tid)
{
  smb_share_t *iter;

  assert(s != NULL && tid);

  iter = s->shares;
  while(iter != NULL && iter->tid != tid)
    iter = iter->next;

  return (iter);
}

smb_share_t *smb_session_share_remove(smb_session_t *s, smb_tid tid)
{
  smb_share_t *iter, *keep;

  assert(s != NULL && tid);
  iter = s->shares;

  if (iter == NULL)
    return (NULL);
  if (iter->tid == tid)
  {
    s->shares = s->shares->next;
    return (iter);
  }

  while(iter->next != NULL && iter->next->tid != tid)
    iter = iter->next;

  if (iter->next != NULL) // We found it
  {
    keep = iter->next;
    iter->next = iter->next->next;
    return (keep);
  }
  return (NULL);
}

int         smb_session_file_add(smb_session_t *s, smb_tid tid, smb_file_t *f)
{
  smb_share_t *share;
  smb_file_t  *iter;

  assert(s != NULL && tid && f != NULL);

  if ((share = smb_session_share_get(s, tid)) == NULL)
    return (0);

  if (share->files == NULL)
    share->files = f;
  else
  {
    iter = share->files;
    while (iter->next != NULL)
      iter = iter->next;
    iter->next = f;
  }

  return (1);
}

smb_file_t  *smb_session_file_get(smb_session_t *s, smb_fd fd)
{
  smb_share_t *share;
  smb_file_t  *iter;

  assert(s != NULL && fd);

  if ((share = smb_session_share_get(s, SMB_FD_TID(fd))) == NULL)
    return (NULL);

  iter = share->files;
  while(iter != NULL && iter->fid != SMB_FD_FID(fd))
    iter = iter->next;

  return (iter);
}

smb_file_t  *smb_session_file_remove(smb_session_t *s, smb_fd fd)
{
  smb_share_t *share;
  smb_file_t  *iter, *keep;

  assert(s != NULL && fd);

  if ((share = smb_session_share_get(s, SMB_FD_TID(fd))) == NULL)
    return (NULL);

  iter = share->files;

  if (iter == NULL)
    return (NULL);
  if (iter->fid == SMB_FD_FID(fd))
  {
    share->files = iter->next;
    return (iter);
  }

  while(iter->next != NULL && iter->next->fid != SMB_FD_TID(fd))
    iter = iter->next;
  if (iter->next != NULL)
  {
    keep = iter->next;
    iter->next = iter->next->next;
    return (keep);
  }
  else
    return (NULL);
}

smb_tid         smb_tree_connect(smb_session_t *s, const char *path)
{
  smb_tree_connect_req_t  *req;
  smb_tree_connect_resp_t *resp;
  smb_message_t   resp_msg;
  smb_message_t   *req_msg;
  smb_share_t     *share;

  assert(s != NULL && path != NULL);

  req_msg = smb_message_new(SMB_CMD_TREE_CONNECT, 128);

  // Packet headers
  smb_message_set_default_flags(req_msg);
  req_msg->packet->header.uid      = s->uid;
  req_msg->packet->header.tree_id  = 0xffff; // Behavior of libsmbclient

  // Packet payload
  req = (smb_tree_connect_req_t *)req_msg->packet->payload;
  smb_message_advance(req_msg, sizeof(smb_tree_connect_req_t));
  req->wct          = 4;
  req->andx         = 0xff;
  req->reserved     = 0;
  req->andx_offset  = 0;
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

  share->tid          = resp_msg.packet->header.tree_id;
  share->opts         = resp->opt_support;
  share->rights       = resp->max_rights;
  share->guest_rights = resp->guest_rights;

  smb_session_share_add(s, share);

  return(share->tid);
}

int           smb_tree_disconnect(smb_session_t *s, smb_tid tid)
{
  assert(s != NULL && tid);

  return (0);
}
