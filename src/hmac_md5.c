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
#include <stdlib.h>
#include <string.h>

#include "bdsm/hmac_md5.h"

unsigned char *HMAC_MD5(const void *key, size_t key_len, const void *msg,
                        size_t msg_len, void *hmac)
{
  static uint8_t  hmac_static[16];

  uint8_t         key_pad[64], o_key_pad[64], i_key_pad[64], kcat[80];
  void            *cat, *out;
  MD5_CTX         ctx;

  assert(key != NULL && msg != NULL);

  // This is Microsoft variation for NTLMv2 (afaik)
  // afaik, they truncate the key instead of hashing it
  if (key_len > 64)
    key_len = 64;
  else
  {
    memcpy(key_pad, key, key_len);
    memset(key_pad + key_len, 0, 64 - key_len);
  }

  // Compute the o/i XORed padded keys
  for (unsigned i = 0; i < 64; i++)
  {
   o_key_pad[i] = 0x5c ^ key_pad[i];
   i_key_pad[i] = 0x36 ^ key_pad[i];
  }

  // Concatenate inner padded key with message
  cat = malloc(msg_len + 64);
  assert (cat != NULL);
  memcpy(cat, i_key_pad, 64);
  memcpy(cat + 64, msg, msg_len);

  // MD5 of the result
  MD5_Init(&ctx);
  MD5_Update(&ctx, cat, msg_len + 64);
  MD5_Final(key_pad, &ctx);
  free(cat);

  memcpy(kcat, o_key_pad, 64);
  memcpy(kcat + 64, key_pad, 16);

  if (hmac != NULL)
    out = hmac;
  else
    out = hmac_static;

  MD5_Init(&ctx);
  MD5_Update(&ctx, kcat, 80);
  MD5_Final(out, &ctx);

  return (out);
}
