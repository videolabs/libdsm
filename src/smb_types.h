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

/**
 * @file smb_types.h
 * @brief liBDSM types and structures
 */

#ifndef _SMB_TYPES_H_
#define _SMB_TYPES_H_

#include <stddef.h>
#include <stdbool.h>

#include <libtasn1.h>

#if !defined _WIN32
# include <netinet/ip.h>
#else
# include <winsock2.h>
#endif

#include "bdsm/smb_types.h"
#include "smb_buffer.h"
#include "smb_packets.h"

/**
 * @internal
 * @struct smb_file
 * @brief An opaque data structure to represent file
 */
struct smb_file
{
    smb_file            *next;          // Next file in this share
    char                *name;
    smb_fid             fid;
    smb_tid             tid;
    size_t              name_len;
    uint64_t            created;
    uint64_t            accessed;
    uint64_t            written;
    uint64_t            changed;
    uint64_t            alloc_size;
    uint64_t            size;
    uint32_t            attr;
    off_t               offset;          // Current position pointer
    int                 is_dir;         // 0 -> file, 1 -> directory
};

typedef struct smb_share smb_share;
struct smb_share
{
    smb_share           *next;          // Next share in this session
    smb_file            *files;         // List of all open files for this share
    smb_tid             tid;
    uint16_t            opts;           // Optionnal support opts
    uint16_t            rights;         // Maximum rights field
    uint16_t            guest_rights;
};

typedef struct smb_transport smb_transport;
struct smb_transport
{
    void              *session;
    void              *(*new)(size_t buf_size);
    int               (*connect)(uint32_t ip, void *s, const char *name);
    void              (*destroy)(void *s);
    void              (*pkt_init)(void *s);
    int               (*pkt_append)(void *s, void *data, size_t size);
    int               (*send)(void *s);
    ssize_t           (*recv)(void *s, void **data);
};

typedef struct smb_srv_info smb_srv_info;
struct smb_srv_info
{
    char                name[16];       // The server name
    uint16_t            dialect;        // The selected dialect
    uint16_t            security_mode;  // Security mode
    uint16_t            uid;            // uid attributed by the server.
    uint32_t            session_key;    // The session key sent by the server on protocol negotiate
    uint32_t            caps;           // Server caps replyed during negotiate
    uint64_t            challenge;      // For challenge response security
    uint64_t            ts;             // It seems Win7 requires it :-/
};

/**
 * @brief An opaque data structure to represent a SMB Session.
 */
struct smb_session
{
    bool                logged;
    bool                guest;            // Are we logged as guest ?

    // Informations about the smb server we are connected to.
    smb_srv_info        srv;


    ASN1_TYPE           spnego_asn1;
    smb_buffer          xsec_target;

    smb_creds           creds;
    smb_transport       transport;

    smb_share           *shares;          // shares->files | Map fd <-> smb_file
    uint32_t            nt_status;
};

typedef struct smb_message smb_message;
struct smb_message
{
    size_t          payload_size; // Size of the allocated payload
    size_t          cursor;       // Write cursor in the payload
    smb_packet      *packet;      // Yummy yummy, Fruity fruity !
};

#endif
