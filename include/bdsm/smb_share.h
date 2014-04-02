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

#ifndef __BDSM_SMB_SHARE_H_
#define __BDSM_SMB_SHARE_H_

#include "bdsm/smb_session.h"

void        smb_session_share_add(smb_session_t *s, smb_share_t *share);
smb_share_t *smb_session_share_get(smb_session_t *s, smb_tid tid);
smb_share_t *smb_session_share_remove(smb_session_t *s, smb_tid tid);

int         smb_session_file_add(smb_session_t *s, smb_tid tid, smb_file_t *f);
smb_file_t  *smb_session_file_get(smb_session_t *s, smb_fd fd);
smb_file_t  *smb_session_file_remove(smb_session_t *s, smb_fd fd);


smb_tid     smb_tree_connect(smb_session_t *s, const char *path);
int         smb_tree_disconnect(smb_session_t *s, smb_tid tid);
smb_fd      smb_fopen(smb_session_t *s, smb_tid tid, const char *path,
                      uint32_t mod);
void        smb_fclose(smb_session_t *s, smb_fd);

#endif
