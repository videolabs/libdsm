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

#define _BSD_SOURCE             /* See feature_test_macros(7) */
#include <endian.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "mdx/md4.h"
#include "rc4/rc4.h"
#include "bdsm/debug.h"
#include "bdsm/smb_ntlm.h"

void hexprint(const char *name, const char *data, size_t data_sz)
{
  printf("%s =", name);

  for(size_t i = 0; i < data_sz; i++)
  {
    if (i % 16 == 0)
    {
      printf("\n0x");
    }
    printf("%.2hhx", *(data + i));
  }
  printf("\n");
}

int main(int argc, char const *argv[])
{
  // const char  *user     = "lta";//"BDSM";
  // const char  *password = "lta";//"qweasd42";
  // const char  *domain   = "MACBOOKPRO-F13E";//"CERBERE";
  const char  *user     = "BDSM";
  const char  *password = "qweasd";
  const char  *domain   = "STELLAR";
  const char  *computer = "COMPUTER";
  //const char  *session_key    = "UUUUUUUUUUUUUUUU";
  //uint64_t    srv_challenge   = htobe64(0xe4de7b5beb9ed434);
  //uint64_t    usr_challenge   = htobe64(0x536843b23b75eb7f);
  uint64_t    srv_challenge   = htobe64(0x80206d74ea232d2c);
  uint64_t    usr_challenge   = htobe64(0x9a12f85759053d89);
  uint64_t    srv_ts          = htobe64(0x80b0dda51669cf01);
  uint64_t    srv_ts2         = htobe64(0x34fd56df1669cf01);
  void        *lm2, *ntlm2;

  smb_ntlm_blob *blob;
  size_t        blob_sz;

  smb_ntlmh   hashv1, hashv2, xkey, xkey_crypt;
  smb_buffer  buf;

  smb_ntlm_hash(password, &hashv1);
  smb_ntlm2_hash(user, password, domain, &hashv2);
  lm2 = smb_lm2_response(&hashv2, srv_challenge, usr_challenge);

  blob_sz = 16;//smb_ntlm_make_blob(&blob, srv_ts, usr_challenge, domain, domain, srv_ts2);
  smb_buffer_init(&buf, lm2, blob_sz);
  ntlm2 = smb_ntlm2_response(&hashv2, srv_challenge, &buf);
  //smb_ntlm2_session_key(&hashv2, ntlm2, &session_key);

  smb_ntlm_generate_xkey(&xkey);
  smb_ntlm2_session_key(&hashv2, ntlm2, &xkey, &xkey_crypt);


  // MD4_CTX ctx;
  // MD4_Init(&ctx);
  // MD4_Update(&ctx, hashv1, 16);
  // MD4_Final(session_key, &ctx);
  hexprint("Srv Challenge", &srv_challenge, 8);
  hexprint("Usr Challenge", &usr_challenge, 8);
  //printf("Server challenge = %.8lx\n", srv_challenge);
  //printf("User challenge = %.8lx\n", usr_challenge);

  printf("Timestamp = %.8"PRIx64"\n", srv_ts);
  printf("-------------------------------------\n");

  hexprint("NT# v1", hashv1, 16);
  hexprint("NT# v2", hashv2, 16);

  hexprint("LMv2 Response", lm2, 24);
  hexprint("NTLMv2 Response", ntlm2, blob_sz + 16);
  hexprint("Session Key", xkey_crypt, 16);

  return 0;
}
