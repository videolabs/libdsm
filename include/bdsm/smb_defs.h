// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

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
#define SMD_CMD_TREE_DISCONNECT 0x71
#define SMB_CMD_NEGOTIATE       0x72
#define SMB_CMD_SETUP           0x73 // Session Setup AndX
#define SMB_CMD_TREE_CONNECT    0x75 // Tree Connect AndX
#define SMB_CMD_ECHO            0x2b
#define SMB_CMD_READ            0x2e // Read AndX
#define SMB_CMD_CREATE          0xa2 // NT Create AndX

#define SMB_SET_FLAG            0
#define SMB_SET_FLAG2           0

#define SMB_FLAG_RESPONSE       (1 << 7)
#define SMB_FLAG_NOTIFY         (1 << 6)
#define SMB_FLAG_OPLOCK         (1 << 5)
#define SMB_FLAG_CANONIC        (1 << 4)
#define SMB_FLAG_CASELESS       (1 << 3)
#define SMB_FLAG_BUFFER_POSTED  (1 << 1)
#define SMB_FLAG_LOCK_AND_READ  (1 << 0)

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

#define NT_STATUS_SUCCESS                   0x00000000
#define NT_STATUS_MORE_PROCESSING_REQUIRED  0xc0000016

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
  uint64_t        time;           // I don't give a fuck
  uint16_t        tz;             // Even less fuck given
  uint8_t         key_length;     // Size of challenge key, if > 8 then shit
  uint16_t        bct;
  uint64_t        challenge;      // Normally 8 bytes, if not then wtf monkey
  uint8_t         payload[];      // The rest isn't really meaningfull for us
} __attribute__((packed))   smb_negotiate_resp_t;

typedef struct
{
  uint8_t         wct;          // +-12 :)
  uint8_t         andx;         // 0xff when no other command (recommended :)
  uint8_t         reserved;     // 0x00
  uint16_t        andx_offset;  // 0x00 when no other command
  uint16_t        max_buffer;   // Maximum size we can receive
  uint16_t        mpx_count;    // maximum multiplexed session
  uint16_t        vc_count;     // Virtual ciruits -> 1!
  uint32_t        session_key;  // 0x00000000
  uint16_t        blob_length;  // Length of Security Blob
  uint32_t        reserved2;    // 0x00000000
  uint32_t        caps;         // Capabilities
  uint16_t        payload_size;
  uint8_t         payload[];
} __attribute__((packed))   smb_session_req_t;

typedef struct
{

} __attribute__((packed))   smb_session_resp_t;

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
  uint16_t        tree_id;      // SMB's file descriptor or service id ?
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
