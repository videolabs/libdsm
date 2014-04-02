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

#ifndef __BDSM_SMB_NTLM_H_
#define __BDSM_SMB_NTLM_H_

#include "bdsm/smb_defs.h"

typedef uint8_t smb_ntlmh_t[SMB_NTLM_HASH_SIZE];

typedef struct
{
  uint32_t    header;
  uint32_t    reserved;
  uint64_t    timestamp;
  uint64_t    challenge;
  uint32_t    unknown;
  uint8_t     domain[];
} __attribute__((packed)) smb_ntlm_blob_t;

uint64_t    smb_ntlm_generate_challenge();
void        smb_ntlm_hash(const char *password, smb_ntlmh_t *hash);
void        smb_ntlm2_hash(const char *username, const char *password,
                           const char *destination, smb_ntlmh_t *hash);
// Returned response is blob_size + 16 long. You'll have to free it
uint8_t     *smb_ntlm2_response(smb_ntlmh_t *hash_v2, uint64_t srv_challenge,
                                uint8_t *blob, size_t blob_size);
// This method seems useless on Win7 (other untested)
size_t      smb_ntlm_blob(smb_ntlm_blob_t *blob, uint64_t ts,
                              uint64_t user_challenge, const char *domain);
#endif
