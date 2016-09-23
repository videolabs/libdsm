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

#ifndef _NETBIOS_DEFS_H_
#define _NETBIOS_DEFS_H_

#include <stdint.h>

#include "bdsm/netbios_defs.h"
#include "bdsm_common.h"

#define NETBIOS_PORT_NAME     137 // UDP
#define NETBIOS_PORT_SESSION  139 // TCP
#define NETBIOS_PORT_DIRECT   445 // TCP
#define NETBIOS_PORT_DIRECT_SECONDARY 139 // TCP

#define NETBIOS_NAME_LENGTH   15

#define NETBIOS_NAME_FLAG_GROUP (1 << 15)

// http://ubiqx.org/cifs/rfc-draft/rfc1001.html#s17.2
#define NETBIOS_WILDCARD      { 32, 'C', 'K', 'A', 'A', 'A', 'A', 'A', 'A',    \
    'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', \
    'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 0 }

#define NETBIOS_FLAG_QUERY      (1 << 15)
#define NETBIOS_FLAG_TRUNCATED  (1 << 9)
#define NETBIOS_FLAG_RECURSIVE  (1 << 8)
#define NETBIOS_FLAG_BROADCAST  (1 << 4)

// Name Service Query
#define NETBIOS_OP_NAME_QUERY         0x00
// Session Service
#define NETBIOS_OP_SESSION_MSG        0x00
#define NETBIOS_OP_SESSION_REQ        0x81
#define NETBIOS_OP_SESSION_REQ_OK     0x82
#define NETBIOS_OP_SESSION_REQ_NOK    0x83
#define NETBIOS_OP_SESSION_RETARGET   0x84
#define NETBIOS_OP_SESSION_KEEPALIVE  0x85

SMB_PACKED_START typedef struct
{
    uint16_t                    trn_id;     // Transaction ID
    uint16_t                    flags;      // Various flags
    uint16_t                    queries;    // Number of queries in this packet
    uint16_t                    answers;    // Number of answers
    uint16_t                    ns_count;   // Number of authorities (?)
    uint16_t                    ar_count;   // Additionnal (??)
    char                        payload[];
} SMB_PACKED_END   netbios_query_packet;

SMB_PACKED_START typedef struct
{
    uint8_t                     opcode;     // 'TYPE'
    uint8_t                     flags;      // 0-6 reserved (== 0), byte 7 is the
    // beginning of the length field (!!)
    uint16_t                    length;     // payload length;
    uint8_t                     payload[];
} SMB_PACKED_END   netbios_session_packet;

#endif
