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

#ifndef __BSDM_SMB_PACKETS_H_
#define __BSDM_SMB_PACKETS_H_

#include <stdint.h>

#define SMB_ANDX_MEMBERS  \
  uint8_t         andx;           /* 0xff when no other command (do this :)*/  \
  uint8_t         andx_reserved;  /* 0x00 */                                   \
  uint16_t        andx_offset;    /* 0x00 when no other command */


////////////////////////////////////////////////////////////////////////////////
// Main structures for holding packet data and building packets

typedef struct
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
} __attribute__((packed))       smb_header_t;

typedef struct
{
  smb_header_t    header;       // A packet header full of gorgeous goodness.
  uint8_t         payload[];    // Ze yummy data inside. Eat 5 fruits/day !
} __attribute__((packed))       smb_packet_t;


////////////////////////////////////////////////////////////////////////////////
// Individual SMB command payload description

//-> Negotiate Protocol
typedef struct
{
  uint8_t         wct; // zero
  uint16_t        bct;
  char            dialects[];

} __attribute__((packed))   smb_negotiate_req_t;

//<- Negotiate Protocol
typedef struct
{
  uint8_t         wct;            // +-17 :)
  uint16_t        dialect_index;  //
  uint8_t         security_mode;  // Share/User. Plaintext/Challenge
  uint32_t        diplodocus;
  uint32_t        max_bufsize;    // Max buffer size requested by server.
  uint32_t        max_rawbuffer;  // Max raw buffer size requested by serv.
  uint32_t        session_key;    // 'MUST' be returned to server
  uint32_t        caps;
  uint64_t        ts;             // I don't give a fuck (or do i?)
  uint16_t        tz;             // Even less fuck given
  uint8_t         key_length;     // Size of challenge key, if != 8 then shit
  uint16_t        bct;
  uint64_t        challenge;      // Normally 8 bytes, if not then wtf monkey
  uint8_t         payload[];      // The rest isn't really meaningfull for us
} __attribute__((packed))   smb_negotiate_resp_t;



//-> Session Setup
typedef struct
{
  uint8_t         wct;          // +-13 :)
  SMB_ANDX_MEMBERS
  uint16_t        max_buffer;   // Maximum size we can receive
  uint16_t        mpx_count;    // maximum multiplexed session
  uint16_t        vc_count;     // Virtual ciruits -> 1!

  uint32_t        session_key;  // 0x00000000
  uint16_t        oem_pass_len; // Length of LM2 response
  uint16_t        uni_pass_len; // Length of NTLM2 response
  uint32_t        reserved2;    // 0x00000000
  uint32_t        caps;         // Capabilities
  uint16_t        payload_size;
  uint8_t         payload[];
} __attribute__((packed))   smb_session_req_t;

//<- Session Setup
typedef struct
{
  uint8_t         wct;
  SMB_ANDX_MEMBERS
  uint16_t        action;
  uint16_t        bct;
  uint8_t         bullshit[];
} __attribute__((packed))   smb_session_resp_t;



//-> Tree Connect
typedef struct
{
  uint8_t         wct;              // 4
  SMB_ANDX_MEMBERS
  uint16_t        flags;
  uint16_t        passwd_len;       // 1 if not used. Used in Share Level Auth
  uint16_t        bct;
  uint8_t         payload[];        // Password | Path | Service

} __attribute__((packed))   smb_tree_connect_req_t;

//<- Tree Connect
typedef struct
{
  uint8_t         wct;              // 7
  SMB_ANDX_MEMBERS
  uint16_t        opt_support;
  uint32_t        max_rights;
  uint32_t        guest_rights;
  uint16_t        bct;
  uint8_t         payload[];
} __attribute__((packed))   smb_tree_connect_resp_t;



//-> Create File
typedef struct
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
} __attribute__((packed))   smb_create_req_t;

//<- Create File
typedef struct
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
} __attribute__((packed))   smb_create_resp_t;



//-> Close File
typedef struct
{
  uint8_t         wct;                // 3
  uint16_t        fid;
  uint32_t        last_write;         // Not defined == 0xffffffff
  uint16_t        bct;                // 0
} __attribute__((packed))   smb_close_req_t;



//-> Read File
typedef struct
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
} __attribute__((packed))   smb_read_req_t;

//<- Read File
typedef struct
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
  uint8_t         padding;
  uint8_t         file[];
} __attribute__((packed))   smb_read_resp_t;

//-> Trans
typedef struct
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
} __attribute__((packed))   smb_trans_req_t;





//-> Trans2
typedef struct
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
} __attribute__((packed))   smb_trans2_req_t;

//// -> Trans2|FindFirst2
typedef struct
{
  uint16_t      attrs;              // Search attributes
  uint16_t      count;              // Search count
  uint16_t      flags;
  uint16_t      interest;           // What kind of info do we want ?
  uint32_t      storage;            // ? => 0
  uint8_t       pattern[];          // The queried pattern "\\folder\\*"
} __attribute__((packed))   smb_tr2_find2_t;

//// -> Trans2|QueryPathInfo
typedef struct
{
  uint16_t      interest;
  uint32_t      reserved;
  uint8_t       path[];
} __attribute__((packed))   smb_tr2_query_t;

//<- Trans2

typedef struct
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
} __attribute__((packed))   smb_trans2_resp_t;

//// <- Trans2|FindFirst2Params
typedef struct
{
  uint16_t      id;
  uint16_t      count;
  uint16_t      eos;
  uint16_t      ea_error_offset;
  uint16_t      last_name_offset;
  uint16_t      padding;
} __attribute__((packed))   smb_tr2_find2_params_t;

//// <- Trans2|FindFirst2FileInfo
typedef struct
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
} __attribute__((packed))   smb_tr2_find2_entry_t;


//// <- Trans2|QueryPathInfo
typedef struct
{
  uint64_t      created;
  uint64_t      accessed;
  uint64_t      written;
  uint64_t      changed;
  uint32_t      attr;
  uint64_t      alloc_size;
  uint64_t      size;
  uint32_t      link_count;
  uint8_t       rm_pending;
  uint8_t       is_dir;
  uint32_t      ea_list_len;
  uint32_t      name_len;
  uint8_t       name[];
} __attribute__((packed))   smb_tr2_path_info_t;


//-> Example
typedef struct
{
  uint8_t         wct;                // ??
  SMB_ANDX_MEMBERS
  // Fill me
  uint16_t        bct;
  //uint8_t         padding;
  uint8_t         file[];
} __attribute__((packed))   smb_example_t;


#endif
