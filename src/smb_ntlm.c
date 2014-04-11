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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "mdx/md4.h"
#include "bdsm/hmac_md5.h"
#include "bdsm/smb_utils.h"
#include "bdsm/smb_ntlm.h"

uint64_t    smb_ntlm_generate_challenge()
{
  uint64_t        result;
  uint64_t        *memory;
  struct timeval  t;

  gettimeofday(&t, NULL);
  srandom(t.tv_usec);
  // Get random data from uninitialized memory, and 'random' memory address
  memory = malloc(sizeof(uint64_t));

  result = random() + (random() << 32) + (uint64_t)memory * *memory;

  return (result);
}

void        smb_ntlm_hash(const char *password, smb_ntlmh_t *hash)
{
  MD4_CTX   ctx;
  char      *ucs2le_pass;
  size_t    sz;

  assert(password != NULL && hash != NULL);

  sz = smb_to_utf16(password, strlen(password), &ucs2le_pass);
  memset((void *)hash, 0, SMB_NTLM_HASH_SIZE);

  MD4_Init(&ctx);
  MD4_Update(&ctx, (uint8_t *)ucs2le_pass, sz);
  MD4_Final((uint8_t *)hash, &ctx);

  free(ucs2le_pass);
}

void        smb_ntlm2_hash(const char *user, const char *password,
                           const char *dest, smb_ntlmh_t *hash)
{
  smb_ntlmh_t   hash_v1;
  char          *ucs_user, *ucs_dest, *data;
  size_t        ucs_user_len, ucs_dest_len, data_len;


  smb_ntlm_hash(password, &hash_v1);

  ucs_user_len  = smb_to_utf16(user, strlen(user), &ucs_user);
  ucs_dest_len  = smb_to_utf16(dest, strlen(dest), &ucs_dest);
  data_len      = ucs_user_len + ucs_dest_len;
  data          = alloca(data_len);

  memcpy(data, ucs_user, ucs_user_len);
  memcpy(data + ucs_user_len, ucs_dest, ucs_dest_len);

  HMAC_MD5(hash_v1, SMB_NTLM_HASH_SIZE, data, data_len, hash);

  free(ucs_user);
  free(ucs_dest);
}

uint8_t     *smb_ntlm2_response(smb_ntlmh_t *hash_v2, uint64_t srv_challenge,
                                uint8_t *blob, size_t blob_size)
{
  struct timeval  t;
  uint8_t         *data, *response, hmac[16];
  size_t          data_len;


  data_len  = sizeof(uint64_t) + blob_size;
  data      = alloca(data_len);
  assert(data != NULL);
  memcpy(data, (void *)&srv_challenge, sizeof(uint64_t));
  memcpy(data + sizeof(uint64_t), blob, blob_size);

  HMAC_MD5(hash_v2, SMB_NTLM_HASH_SIZE, data, data_len, &hmac);

  response = malloc(blob_size + 16);
  assert(response != NULL);
  memcpy(response, (void *)hmac, 16);
  memcpy(response + 16, blob, blob_size);

  return (response);
}

size_t      smb_ntlm_blob(smb_ntlm_blob_t *blob, uint64_t ts,
                          uint64_t user_challenge, const char *domain)
{
  char      *ucs_domain;
  size_t    ucs_domain_len;

  assert(blob != NULL && domain != NULL);

  memset((void *)blob, 0, sizeof(smb_ntlm_blob_t));
  blob->header    = 0x101;
  blob->timestamp = ts;
  blob->challenge = user_challenge;

  ucs_domain_len = smb_to_utf16(domain, strlen(domain), &ucs_domain);
  *((uint16_t *)blob->domain)     = 0x0002;   // type == Netbios domain
  *((uint16_t *)blob->domain + 1) = ucs_domain_len;

  memcpy(blob->domain + 4, (void *)ucs_domain, ucs_domain_len);
  memset(blob->domain + 4 + ucs_domain_len, 0, 4);
  free(ucs_domain);

  return (sizeof(smb_ntlm_blob_t) + ucs_domain_len + 8);
}
