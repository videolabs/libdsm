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

#ifndef __BSDM_SMB_DEFS_H_
#define __BSDM_SMB_DEFS_H_

#include <stdint.h>

#define SMB_MAGIC               { 0xff, 0x53, 0x4d, 0x42 } // aka "\xffSMB"

#define SMB_DIALECTS {          \
  "\2Samba",                    \
  "\2NT LM 0.12",               \
  NULL                          \
}

// Dialect values must match position on SMB_DIALECTS array
#define SMB_DIALECT_SAMBA       0
#define SMB_DIALECT_NTLM        1

#define SMB_CMD_CLOSE           0x04
#define SMB_CMD_TRANS2          0x32
#define SMB_CMD_TREE_DISCONNECT 0x71
#define SMB_CMD_NEGOTIATE       0x72
#define SMB_CMD_SETUP           0x73 // Session Setup AndX
#define SMB_CMD_TREE_CONNECT    0x75 // Tree Connect AndX
#define SMB_CMD_ECHO            0x2b
#define SMB_CMD_READ            0x2e // Read AndX
#define SMB_CMD_CREATE          0xa2 // NT Create AndX

///////////////////////////////////////////////////////////////////////////////
//// Flags definitions

// Many aren't use in libdsm but are here for possible later use

// Protocol negotiation flags (flags field in spec)
#define SMB_FLAG_RESPONSE       (1 << 7)
#define SMB_FLAG_NOTIFY         (1 << 6)
#define SMB_FLAG_OPLOCK         (1 << 5)
#define SMB_FLAG_CANONIC        (1 << 4)
#define SMB_FLAG_CASELESS       (1 << 3)
#define SMB_FLAG_BUFFER_POSTED  (1 << 1)
#define SMB_FLAG_LOCK_AND_READ  (1 << 0)
// More Protocol negotiation flags (flags2 field in spec)
#define SMB_FLAG_UNICODE        (1 << (15 + 8))
#define SMB_FLAG_NT_ERRORS      (1 << (14 + 8))
#define SMB_FLAG_EXECUTE_ONLY   (1 << (13 + 8))
#define SMB_FLAG_DFS            (1 << (12 + 8))
#define SMB_FLAG_EXT_SEC        (1 << (11 + 8))
#define SMB_FLAG_REPARSE_PATH   (1 << (10 + 8))
#define SMB_FLAG_LONG_NAMES     (1 << (6 + 8))
#define SMB_FLAG_SIGN_REQUIRED  (1 << (4 + 8))
#define SMB_FLAG_COMPRESSED     (1 << (3 + 8))
#define SMB_FLAG_SIGN_SUPPORT   (1 << (2 + 8))
#define SMB_FLAG_EXT_ATTR       (1 << (1 + 8))
#define SMB_FLAG_LONG_NAMES_OK  (1 << (0 + 8))
// File creation/open flags
#define SMB_CREATE_OPLOCK       (1 << 1)
#define SMB_CREATE_BATCH_OPLOCK (1 << 2)
#define SMB_CREATE_MKDIR        (1 << 3)
#define SMB_CREATE_EXT_RESP     (1 << 4)
#define SMB_CREATE_DEFAULTS     (0)
// File access rights
#define SMB_MOD_READ            (1 << 0)
#define SMB_MOD_WRITE           (1 << 1)
#define SMB_MOD_APPEND          (1 << 2)
#define SMB_MOD_READ_EXT        (1 << 3)
#define SMB_MOD_WRITE_EXT       (1 << 4)
#define SMB_MOD_EXEC            (1 << 5)
#define SMB_MOD_RMCHILD         (1 << 6)
#define SMB_MOD_READ_ATTR       (1 << 7)
#define SMB_MOD_WRITE_ATTR      (1 << 8)
#define SMB_MOD_RM              (1 << 16)
#define SMB_MOD_READ_CTL        (1 << 17)
#define SMB_MOD_WRITE_DAC       (1 << 18)
#define SMB_MOD_CHOWN           (1 << 19)
#define SMB_MOD_SYNC            (1 << 20)
#define SMB_MOD_SYS             (1 << 24)
#define SMB_MOD_MAX_ALLOWED     (1 << 25)
#define SMB_MOD_GENERIC_ALL     (1 << 28)
#define SMB_MOD_GENERIC_EXEC    (1 << 29)
#define SMB_MOD_GENERIC_READ    (1 << 30)
#define SMB_MOD_GENERIC_WRITE   (1 << 31)
#define SMB_MOD_RW              (SMB_MOD_READ | SMB_MOD_WRITE | SMB_MOD_APPEND \
                                | SMB_MOD_READ_EXT | SMB_MOD_WRITE_EXT \
                                | SMB_MOD_READ_ATTR | SMB_MOD_WRITE_ATTR \
                                | SMB_MOD_READ_CTL )
#define SMB_MOD_RO              (SMB_MOD_READ | SMB_MOD_READ_EXT \
                                | SMB_MOD_READ_ATTR | SMB_MOD_READ_CTL )

// File attributes
#define SMB_ATTR_RO             (1 << 0)
#define SMB_ATTR_HIDDEN         (1 << 1)
#define SMB_ATTR_SYS            (1 << 2)
#define SMB_ATTR_VOLID          (1 << 3)  // Volume ID
#define SMB_ATTR_DIR            (1 << 4)
#define SMB_ATTR_ARCHIVE        (1 << 5)  // Modified since last archive (!?)
#define SMB_ATTR_DEVICE         (1 << 6)
#define SMB_ATTR_NORMAL         (1 << 7)
#define SMB_ATTR_TEMP           (1 << 8)
#define SMB_ATTR_SPARSE         (1 << 9)
#define SMB_ATTR_REPARSE_PT     (1 << 10)
#define SMB_ATTR_COMPRESSED     (1 << 11)
#define SMB_ATTR_OFFLINE        (1 << 12)
#define SMB_ATTR_INDEXED        (1 << 13) // Not set = May be indexed
#define SMB_ATTR_ENCRYPTED      (1 << 14)
// Share access flags
#define SMB_SHARE_READ          (1 << 0)
#define SMB_SHARE_WRITE         (1 << 1)
#define SMB_SHARE_DELETE        (1 << 2)





#define SMB_NTLM_HASH_SIZE      16
#define SMB_NTLM2_BLOB_SIZE     64
#define SMB_LM2_BLOB_SIZE       8

#define SMB_OS                  "Unix"
#define SMB_LANMAN              "libdsm-dev"

#define NT_STATUS_SUCCESS                   0x00000000
#define NT_STATUS_MORE_PROCESSING_REQUIRED  0xc0000016
#define NT_STATUS_ACCESS_DENIED             0xc0000022

#define SMB_ANDX_MEMBERS  \
  uint8_t         andx;         /* 0xff when no other command (recommended :)*/\
  uint8_t         reserved;     /* 0x00 */                                     \
  uint16_t        andx_offset;  /* 0x00 when no other command */


////////////////////////////////////////////////////////////////////////////////
// Individual SMB command payload description


//// Negotiate Protocol

typedef struct
{
  uint8_t         wct; // zero
  uint16_t        bct;
  char            dialects[];

} __attribute__((packed))   smb_negotiate_req_t;

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



//// Session Setup

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

typedef struct
{
  uint8_t         wct;
  SMB_ANDX_MEMBERS
  uint16_t        action;
  uint16_t        bct;
  uint8_t         bullshit[];
} __attribute__((packed))   smb_session_resp_t;



//// Tree Connect

typedef struct
{
  uint8_t         wct;              // 4
  SMB_ANDX_MEMBERS
  uint16_t        flags;
  uint16_t        passwd_len;       // 1 if not used. Used in Share Level Auth
  uint16_t        bct;
  uint8_t         payload[];        // Password | Path | Service

} __attribute__((packed))   smb_tree_connect_req_t;

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



//// Create (or Open) File

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


//// Close File

typedef struct
{
  uint8_t         wct;                // 3
  uint16_t        fid;
  uint32_t        last_write;         // Not defined == 0xffffffff
  uint16_t        bct;                // 0
} __attribute__((packed))   smb_close_req_t;



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


#endif
