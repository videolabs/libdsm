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

#ifndef __BDSM_SMB_FILE_H_
#define __BDSM_SMB_FILE_H_

#include "bdsm/smb_session.h"

smb_fd    smb_fopen(smb_session_t *s, smb_tid tid, const char *path,
                    uint32_t mod);
void      smb_fclose(smb_session_t *s, smb_fd);
ssize_t   smb_fread(smb_session_t *s, smb_fd fd, void *buf, size_t buf_size);
ssize_t   smb_fseek(smb_session_t *s, smb_fd fd, ssize_t offset, int whence);

#endif
