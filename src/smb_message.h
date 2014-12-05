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

#ifndef _SMB_MESSAGE_H_
#define _SMB_MESSAGE_H_

#include "smb_defs.h"
#include "smb_types.h"

smb_message     *smb_message_new(uint8_t cmd, size_t payload_size);
smb_message     *smb_message_grow(smb_message *msg, size_t size);
void            smb_message_destroy(smb_message *msg);
int             smb_message_advance(smb_message *msg, size_t size);
int             smb_message_append(smb_message *msg, const void *data,
                                   size_t data_size);
int             smb_message_put8(smb_message *msg, uint8_t data);
int             smb_message_put16(smb_message *msg, uint16_t data);
int             smb_message_put32(smb_message *msg, uint32_t data);
int             smb_message_put64(smb_message *msg, uint64_t data);
size_t          smb_message_put_utf16(smb_message *msg, const char *str,
                                      size_t str_len);
int             smb_message_put_uuid(smb_message *msg, uint32_t a, uint16_t b,
                                     uint16_t c, const uint8_t e[8]);

void            smb_message_set_andx_members(smb_message *msg);
void            smb_message_flag(smb_message *msg, uint32_t flag, int value);
#endif
