// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_NETBIOS_DEFS_H_
#define __BDSM_NETBIOS_DEFS_H_

#include <stdint.h>

#define NETBIOS_UDP_PORT      137 // NS Port
#define NETBIOS_TCP_PORT      138 // Session Port

#define NETBIOS_NAME_LENGTH   15

// Netbios name types
#define NETBIOS_WORKSTATION   0x00
#define NETBIOS_MESSENGER     0x03
#define NETBIOS_FILESERVER    0x20
#define NETBIOS_DOMAINMASTER  0x1b

typedef struct              netbios_query_s {
  uint16_t                    rtn_id;     // Transaction ID
  uint16_t                    flags;      // Various flags
  uint16_t                    queries;    // Number of queries in this packet
  uint16_t                    answers;    // Number of answers
  uint16_t                    ns_count;   // Number of authorities (?)
  uint16_t                    ar_count;   // Additionnal (??)
  char                        payload[];
} __attribute__((packed))   netbios_query_t;

// Crazy encoded level 2 netbios name (null-terminated)
typedef char  *netbios_name_t;


#endif
