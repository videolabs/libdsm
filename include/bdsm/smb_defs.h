// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BSDM_SMB_DEFS_H_
#define __BSDM_SMB_DEFS_H_

#include <stdint.h>

#define SMB_MAGIC               { 0xff, 0x53, 0x4d, 0x42 } // aka "\xffSMB"

#define SMB_DIALECTS {          \
  "\2PC NETWORK PROGRAM 1.0",   \
  "\2NT LM 0.12",               \
  NULL                          \
}

#define SMB_CMD_CLOSE           0x04
#define SMB_CMD_TRANS2          0x32
#define SMD_CMD_TREE_DISCONNECT 0x71
#define SMB_CMD_NEGOTIATE       0x72
#define SMB_CMD_SETUP           0x73 // Session Setup AndX
#define SMB_CMD_TREE_CONNECT    0x75 // Tree Connect AndX
#define SMB_CMD_ECHO            0x2b
#define SMB_CMD_READ            0x2e // Read AndX
#define SMB_CMD_CREATE          0xa2 // NT Create AndX

typedef struct
{
  uint8_t         wct; // zero
  uint16_t        bct;
  char            dialects[];

} __attribute__((packed))   smb_negotiate_req_t;

typedef struct
{
  uint8_t         wct; // +-17 :)
  uint8_t         payload[];
} __attribute__((packed))   smb_negotiate_resp_t;

typedef struct
{

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

// This is used with convenience functions to build packets.
typedef struct
{
  size_t          payload_size; // Size of the allocated payload
  size_t          cursor;       // Write cursor in the payload
  smb_packet_t    *packet;      // Yummy yummy, Fruity fruity !
}                               smb_message_t;


#endif
