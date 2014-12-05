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

#ifndef _SMB_FD_H_
#define _SMB_FD_H_

#include "smb_session.h"
#include "smb_message.h"

void            smb_session_share_add(smb_session *s, smb_share *share);
smb_share       *smb_session_share_get(smb_session *s, smb_tid tid);
smb_share       *smb_session_share_remove(smb_session *s, smb_tid tid);
void            smb_session_share_clear(smb_session *s);

int             smb_session_file_add(smb_session *s, smb_tid tid, smb_file *f);
smb_file        *smb_session_file_get(smb_session *s, smb_fd fd);
smb_file        *smb_session_file_remove(smb_session *s, smb_fd fd);

#endif
