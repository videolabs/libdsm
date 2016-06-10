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

#ifndef _SMB_PACKETS_H_
#define _SMB_PACKETS_H_

#include <stdint.h>

#include "bdsm_common.h"

#define SMB_ANDX_MEMBERS  \
  uint8_t         andx;           /* 0xff when no other command (do this :)*/  \
  uint8_t         andx_reserved;  /* 0x00 */                                   \
  uint16_t        andx_offset;    /* 0x00 when no other command */


////////////////////////////////////////////////////////////////////////////////
// Main structures for holding packet data and building packets

SMB_PACKED_START typedef struct
{
    uint8_t         magic[4];     // { 0xff, 0x53, 0x4d, 0x42 } "\xffSMB"
    uint8_t         command;      // The actual SMB command
    uint32_t        status;       // 'NT Status'
    uint8_t         flags;        // Packet flags
    uint16_t        flags2;       // More flags ? (lol)
    uint16_t        pid_high;     // Unused ?
    uint64_t        signature;    // Unused ?
    uint16_t        reserved;     // More usuned bit (we have so much BW :)
    uint16_t        tid;          // A kind of fd for share. (tree_connect)
    uint16_t        pid;          // Process ID.
    uint16_t        uid;          // User ID.
    uint16_t        mux_id;       // Multiplex ID. Increment it sometimes.
} SMB_PACKED_END        smb_header;

SMB_PACKED_START typedef struct
{
    smb_header    header;       // A packet header full of gorgeous goodness.
    uint8_t         payload[];    // Ze yummy data inside. Eat 5 fruits/day !
} SMB_PACKED_END       smb_packet;


////////////////////////////////////////////////////////////////////////////////
// Individual SMB command payload description

// Simple structure used for several requests/responses
SMB_PACKED_START typedef struct
{
    uint8_t         wct;              // 0
    uint16_t        bct;
} SMB_PACKED_END   smb_simple_struct;


//-> Negotiate Protocol
SMB_PACKED_START typedef struct
{
    uint8_t         wct; // zero
    uint16_t        bct;
    char            dialects[];

} SMB_PACKED_END   smb_nego_req;


#define SMB_NEGO_RESP_COMMON \
  uint8_t         wct;            /* +-17 :) */                                \
  uint16_t        dialect_index;                                               \
  uint8_t         security_mode;  /* Share/User. Plaintext/Challenge */        \
  uint32_t        diplodocus;                                                  \
  uint32_t        max_bufsize;    /* Max buffer size requested by server. */   \
  uint32_t        max_rawbuffer;  /* Max raw buffer size requested by serv. */ \
  uint32_t        session_key;    /* 'MUST' be returned to server */           \
  uint32_t        caps;                                                        \
  uint64_t        ts;             /* I don't give a fuck (or do i?) */         \
  uint16_t        tz;             /* Even less fuck given */                   \
  uint8_t         key_length;     /* Size of challenge key // GSS blob */      \
  uint16_t        bct;

//<- Negotiate Protocol
SMB_PACKED_START typedef struct
{
    SMB_NEGO_RESP_COMMON
    uint64_t        challenge;      // Normally 8 bytes, if not then wtf monkey
    uint8_t         payload[];      // The rest isn't really meaningfull for us
} SMB_PACKED_END   smb_nego_resp;

//<- Negotiate Protocol
SMB_PACKED_START typedef struct
{
    SMB_NEGO_RESP_COMMON
    uint8_t         srv_guid[16];
    uint8_t         gssapi[];
} SMB_PACKED_END   smb_nego_xsec_resp;

#define SMB_SESSION_REQ_COMMON \
  uint8_t         wct;          /* +-13 :) */                                  \
  SMB_ANDX_MEMBERS                                                             \
  uint16_t        max_buffer;   /* Maximum size we can receive */              \
  uint16_t        mpx_count;    /* maximum multiplexed session */              \
  uint16_t        vc_count;     /* Virtual ciruits -> 1! */                    \
  uint32_t        session_key;  /* 0x00000000 */

//-> Session Setup
SMB_PACKED_START typedef struct
{
    SMB_SESSION_REQ_COMMON
    uint16_t        oem_pass_len; // Length of LM2 response
    uint16_t        uni_pass_len; // Length of NTLM2 response
    uint32_t        reserved2;    // 0x00000000
    uint32_t        caps;         // Capabilities
    uint16_t        payload_size;
    uint8_t         payload[];
} SMB_PACKED_END   smb_session_req;

//-> Session Setup
SMB_PACKED_START typedef struct
{
    SMB_SESSION_REQ_COMMON
    uint16_t        xsec_blob_size; // Length of GSSAPI/SPNEGO blob
    uint32_t        reserved2;      // 0x00000000
    uint32_t        caps;           // Capabilities
    uint16_t        payload_size;
    uint8_t         payload[];
} SMB_PACKED_END   smb_session_xsec_req;


//<- Session Setup
SMB_PACKED_START typedef struct
{
    uint8_t         wct;
    SMB_ANDX_MEMBERS
    uint16_t        action;
    uint16_t        bct;
    uint8_t         bullshit[];
} SMB_PACKED_END   smb_session_resp;

SMB_PACKED_START typedef struct
{
    uint8_t         wct;
    SMB_ANDX_MEMBERS
    uint16_t        action;
    uint16_t        xsec_blob_size;
    uint16_t        payload_size;
    uint8_t         payload[];
} SMB_PACKED_END   smb_session_xsec_resp;



//-> Tree Connect
SMB_PACKED_START typedef struct
{
    uint8_t         wct;              // 4
    SMB_ANDX_MEMBERS
    uint16_t        flags;
    uint16_t        passwd_len;       // 1 if not used. Used in Share Level Auth
    uint16_t        bct;
    uint8_t         payload[];        // Password | Path | Service

} SMB_PACKED_END   smb_tree_connect_req;

//<- Tree Connect
SMB_PACKED_START typedef struct
{
    uint8_t         wct;              // 7
    SMB_ANDX_MEMBERS
    uint16_t        opt_support;
    uint32_t        max_rights;
    uint32_t        guest_rights;
    uint16_t        bct;
    uint8_t         payload[];
} SMB_PACKED_END   smb_tree_connect_resp;

//-> Tree Disconnect / <- Tree Disconnect
typedef smb_simple_struct smb_tree_disconnect_req;
typedef smb_simple_struct smb_tree_disconnect_resp;

//-> Create File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 24
    SMB_ANDX_MEMBERS
    uint8_t         reserved2;
    uint16_t        path_length;
    uint32_t        flags;
    uint32_t        root_fid;
    uint32_t        access_mask;
    uint64_t        alloc_size;
    uint32_t        file_attr;
    uint32_t        share_access;
    uint32_t        disposition;
    uint32_t        create_opts;
    uint32_t        impersonation;
    uint8_t         security_flags;
    uint16_t        bct;
    uint8_t         path[];             // UTF16 Path, starting with '\'
} SMB_PACKED_END   smb_create_req;

//<- Create File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 34
    SMB_ANDX_MEMBERS
    uint8_t         oplock_level;
    uint16_t        fid;
    uint32_t        action;
    uint64_t        created;            // File creation time
    uint64_t        accessed;           // File last access time
    uint64_t        written;            // File last write time
    uint64_t        changed;            // File last modification time
    uint32_t        attr;
    uint64_t        alloc_size;
    uint64_t        size;
    uint16_t        filetype;
    uint16_t        ipc_state;
    uint8_t         is_dir;
    uint16_t        bct;                // 0
} SMB_PACKED_END   smb_create_resp;



//-> Close File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 3
    uint16_t        fid;
    uint32_t        last_write;         // Not defined == 0xffffffff
    uint16_t        bct;                // 0
} SMB_PACKED_END   smb_close_req;



//-> Read File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 12
    SMB_ANDX_MEMBERS
    uint16_t        fid;
    uint32_t        offset;
    uint16_t        max_count;
    uint16_t        min_count;
    uint32_t        max_count_high;     // Continuation of max_count field
    uint16_t        remaining;
    uint32_t        offset_high;        // Continuation of offset field'
    uint16_t        bct;                // 0
} SMB_PACKED_END   smb_read_req;

//<- Read File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 12
    SMB_ANDX_MEMBERS
    uint16_t        remaining;
    uint16_t        compact_mode;
    uint16_t        reserved;
    uint16_t        data_len;
    uint16_t        data_offset;
    uint32_t        data_len_high;
    uint32_t        reserved2;
    uint16_t        reserved3;
    uint16_t        bct;
} SMB_PACKED_END   smb_read_resp;

//-> Write File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 14
    SMB_ANDX_MEMBERS
    uint16_t        fid;
    uint32_t        offset;
    uint32_t        timeout;
    uint16_t        write_mode;
    uint16_t        remaining;
    uint16_t        reserved;
    uint16_t        data_len;
    uint16_t        data_offset;
    uint32_t        offset_high;        // Continuation of offset field'
    uint16_t        bct;
    uint8_t         padding;
} SMB_PACKED_END   smb_write_req;

//<- Write File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 6
    SMB_ANDX_MEMBERS
    
    uint16_t        data_len;
    uint16_t        available;
    uint32_t        reserved;
    uint16_t        bct;
} SMB_PACKED_END   smb_write_resp;

//-> Remove File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 0x01
    uint16_t        search_attributes;  // 0x0000 for "normal" (not hidden/ystem) files
    uint16_t        bct;                // >= 2
    uint8_t         buffer_format;      // 0x04
} SMB_PACKED_END   smb_file_rm_req;

//<- Remove File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 0x00
    uint16_t        bct;                // 0x0000
} SMB_PACKED_END   smb_file_rm_resp;

//-> Remove Directory
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 0x00
    uint16_t        bct;                // >= 2
    uint8_t         buffer_format;      // 0x04
} SMB_PACKED_END   smb_directory_rm_req;

//<- Remove Directory
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 0x00
    uint16_t        bct;                // 0x0000
} SMB_PACKED_END   smb_directory_rm_resp;

//-> Move File
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 0x01
    uint16_t        search_attributes;  // 0x0000 for "normal" (not hidden/ystem) files
    uint16_t        bct;                // >= 2
} SMB_PACKED_END   smb_file_mv_req;

//<- Move File
typedef smb_simple_struct smb_file_mv_resp;

//-> Create Directory
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // 0x00
    uint16_t        bct;                // >= 2
    uint8_t         buffer_format;      // 0x04
} SMB_PACKED_END   smb_directory_mk_req;

//<- Create Directory
typedef smb_simple_struct smb_directory_mk_resp;

//-> Trans
SMB_PACKED_START typedef struct
{
    uint8_t       wct;                    // 16
    uint16_t      total_param_count;
    uint16_t      total_data_count;
    uint16_t      max_param_count;
    uint16_t      max_data_count;
    uint8_t       max_setup_count;
    uint8_t       reserved;
    uint16_t      flags;
    uint32_t      timeout;
    uint16_t      reserved2;
    uint16_t      param_count;
    uint16_t      param_offset;
    uint16_t      data_count;
    uint16_t      data_offset;
    uint8_t       setup_count;
    uint8_t       reserved3;
    uint16_t      pipe_function;
    uint16_t      fid;
    uint16_t      bct;
    uint8_t       payload[];
} SMB_PACKED_END   smb_trans_req;





//-> Trans2
SMB_PACKED_START typedef struct
{
    uint8_t       wct;                // 15
    uint16_t      total_param_count;
    uint16_t      total_data_count;
    uint16_t      max_param_count;
    uint16_t      max_data_count;
    uint8_t       max_setup_count;
    uint8_t       reserved;
    uint16_t      flags;
    uint32_t      timeout;
    uint16_t      reserve2;
    uint16_t      param_count;
    uint16_t      param_offset;
    uint16_t      data_count;
    uint16_t      data_offset;
    uint8_t       setup_count;
    uint8_t       reserved3;
    uint16_t      cmd;
    uint16_t      bct;
    uint8_t       padding[3];
    uint8_t       payload[];
} SMB_PACKED_END   smb_trans2_req;

//// -> Trans2|FindFirst2
SMB_PACKED_START typedef struct
{
    uint16_t      attrs;              // Search attributes
    uint16_t      count;              // Search count
    uint16_t      flags;
    uint16_t      interest;           // What kind of info do we want ?
    uint32_t      storage;            // ? => 0
    uint8_t       pattern[];          // The queried pattern "\\folder\\*"
} SMB_PACKED_END   smb_tr2_findfirst2;

//// -> Trans2|FindNext2
SMB_PACKED_START typedef struct
{
    uint16_t      sid;                // Search handle
    uint16_t      count;              // Search count
    uint16_t      interest;           // What kind of info do we want ?
    uint32_t      resume_key;         // Value returned by previous find2 call
    uint16_t      flags;
    uint8_t       pattern[];          // The queried pattern "\\folder\\*"
} SMB_PACKED_END   smb_tr2_findnext2;

//// -> Trans2|QueryPathInfo
SMB_PACKED_START typedef struct
{
    uint16_t      interest;
    uint32_t      reserved;
    uint8_t       path[];
} SMB_PACKED_END   smb_tr2_query;

//<- Trans2

SMB_PACKED_START typedef struct
{
    uint8_t       wct;                // 10
    uint16_t      total_param_count;
    uint16_t      total_data_count;
    uint16_t      reserved;
    uint16_t      param_count;
    uint16_t      param_offset;
    uint16_t      param_displacement; // ??
    uint16_t      data_count;
    uint16_t      data_offset;
    uint16_t      data_displacement;  // ??
    uint8_t       setup_count;
    uint8_t       reserved2;
    uint16_t      bct;
    uint8_t       padding;
    uint8_t       payload[];
} SMB_PACKED_END   smb_trans2_resp;

//// <- Trans2|FindFirst2Params
SMB_PACKED_START typedef struct
{
    uint16_t      id;
    uint16_t      count;
    uint16_t      eos;
    uint16_t      ea_error_offset;
    uint16_t      last_name_offset;
    uint16_t      padding;
} SMB_PACKED_END   smb_tr2_findfirst2_params;

//// <- Trans2|FindNext2Params
SMB_PACKED_START typedef struct
{
    uint16_t      count;
    uint16_t      eos;
    uint16_t      ea_error_offset;
    uint16_t      last_name_offset;
} SMB_PACKED_END   smb_tr2_findnext2_params;

//// <- Trans2|FindFirst2FileInfo
SMB_PACKED_START typedef struct
{
    uint32_t      next_entry;
    uint32_t      index;
    uint64_t      created;            // File creation time
    uint64_t      accessed;           // File last access time
    uint64_t      written;            // File last write time
    uint64_t      changed;            // File last modification time
    uint64_t      size;
    uint64_t      alloc_size;
    uint32_t      attr;
    uint32_t      name_len;
    uint32_t      ea_list_len;
    uint8_t       short_name_len;
    uint8_t       reserved;
    uint8_t       short_name[24];
    uint8_t       name[];
} SMB_PACKED_END   smb_tr2_find2_entry;


//// <- Trans2|QueryPathInfo
SMB_PACKED_START typedef struct
{
    uint64_t      created;
    uint64_t      accessed;
    uint64_t      written;
    uint64_t      changed;
    uint32_t      attr;
    uint32_t      reserved;
    uint64_t      alloc_size;
    uint64_t      size;
    uint32_t      link_count;
    uint8_t       rm_pending;
    uint8_t       is_dir;
    uint16_t      reserved2;
    uint32_t      ea_list_len;
    uint32_t      name_len;
    uint8_t       name[];
} SMB_PACKED_END   smb_tr2_path_info;

//-> Example
SMB_PACKED_START typedef struct
{
    uint8_t         wct;                // ??
    SMB_ANDX_MEMBERS
    // Fill me
    uint16_t        bct;
    //uint8_t         padding;
    uint8_t         file[];
} SMB_PACKED_END   smb_example_t;


#endif
