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

#ifndef _SMB_MESSAGE_H_
#define _SMB_MESSAGE_H_

#include "smb_defs.h"
#include "smb_types.h"

smb_message     *smb_message_new(uint8_t cmd);
smb_message     *smb_message_grow(smb_message *msg, size_t size);
void            smb_message_destroy(smb_message *msg);
int             smb_message_advance(smb_message *msg, size_t size);
int             smb_message_append(smb_message *msg, const void *data,
                                   size_t data_size);
int             smb_message_insert(smb_message *msg, size_t cursor,
                                   const void *data, size_t data_size);
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

#define SMB_MSG_INIT_PKT(pkt) do { \
    memset(&pkt, 0, sizeof(pkt)); \
} while (0)

#define SMB_MSG_INIT_PKT_ANDX(pkt) do { \
    SMB_MSG_INIT_PKT(pkt); \
    pkt.andx           = 0xff; \
    pkt.andx_reserved  = 0; \
    pkt.andx_offset    = 0; \
} while (0)

#define SMB_MSG_PUT_PKT(msg, pkt) \
    smb_message_append(msg, &pkt, sizeof(pkt))

#define SMB_MSG_ADVANCE_PKT(msg, pkt) \
    smb_message_advance(msg, sizeof(pkt))

#define SMB_MSG_INSERT_PKT(msg, cursor, pkt) \
    smb_message_insert(msg, cursor, &pkt, sizeof(pkt))

#endif
