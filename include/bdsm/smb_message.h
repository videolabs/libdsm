// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_SMB_PACKET_H_
#define __BDSM_SMB_PACKET_H_

// The actual packet data format definition is here
#include "bdsm/smb_defs.h"

smb_message_t   *smb_message_new(uint8_t cmd, size_t payload_size);
void            smb_message_destroy(smb_message_t *msg);
int             smb_message_append(smb_message_t *msg, const void *data,
                                   size_t data_size);
int             smb_message_put8(smb_message_t *msg, uint8_t data);
int             smb_message_put16(smb_message_t *msg, uint16_t data);
int             smb_message_put32(smb_message_t *msg, uint32_t data);
int             smb_message_put64(smb_message_t *msg, uint32_t data);
#endif
