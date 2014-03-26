// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BSDM_SMB_DEFS_H_
#define __BSDM_SMB_DEFS_H_

#include <stdint.h>

typedef struct
{
  uint8_t         wct; // zero
  uint16_t        bct;
  char            dialects[];

} __attribute__((packed))   smb_negotiate_req_t;

typedef struct
{

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
  uint16_t        uid;          // Process ID.
  uint16_t        mux_id;       // Multiplex ID. Increment it sometimes.
  //uint8_t         payload[];    // The yummy data inside. Remember to eat 5 fruits/day
  union {
    smb_negotiate_req_t     negotiate_req;
    smb_negotiate_resp_t    negotiate_resp;
    smb_session_req_t       session_req;
    smb_session_resp_t      session_resp;
  }               msg;
} __attribute__((packed))   smb_header_t;


#endif
