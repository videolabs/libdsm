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
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <bsd/string.h>

#include "mdx/md4.h"
#include "rc4/rc4.h"
#include "bdsm/debug.h"
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

void        smb_ntlm_generate_xkey(smb_ntlmh *cli_session_key)
{
  uint64_t  *key = (uint64_t *)cli_session_key;

  key[0] = smb_ntlm_generate_challenge();
  key[1] = smb_ntlm_generate_challenge();
}

void        smb_ntlm_hash(const char *password, smb_ntlmh *hash)
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

static void _upcase(char *str)
{
  assert (str != NULL);

  while (*str)
  {
    if (isalpha(*str))
      *str = toupper(*str);
    str++;
  }
}

void        smb_ntlm2_hash(const char *user, const char *password,
                           const char *dest, smb_ntlmh *hash)
{
  smb_ntlmh     hash_v1;
  char          *ucs_user, *ucs_dest, *data, user_upper[64];
  size_t        ucs_user_len, ucs_dest_len, data_len;

  smb_ntlm_hash(password, &hash_v1);

  strlcpy(user_upper, user, 64);
  _upcase(user_upper);

  ucs_user_len  = smb_to_utf16(user_upper, strlen(user_upper), &ucs_user);
  ucs_dest_len  = smb_to_utf16(dest, strlen(dest), &ucs_dest);
  data_len      = ucs_user_len + ucs_dest_len;
  data          = alloca(data_len);

  memcpy(data, ucs_user, ucs_user_len);
  memcpy(data + ucs_user_len, ucs_dest, ucs_dest_len);

  HMAC_MD5(hash_v1, SMB_NTLM_HASH_SIZE, data, data_len, hash);

  free(ucs_user);
  free(ucs_dest);
}

uint8_t     *smb_ntlm2_response(smb_ntlmh *hash_v2, uint64_t srv_challenge,
                                uint8_t *blob, size_t blob_size)
{
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

uint8_t     *smb_lm2_response(smb_ntlmh *hash_v2, uint64_t srv_challenge,
                              uint64_t user_challenge)
{
  smb_ntlm2_response(hash_v2, srv_challenge, &user_challenge, 8);
}

static void   _wcamelcase(char *str)
{
  int first = 1;

  assert (str != NULL);

  while(*str)
  {
    if (isalpha(*str))
    {
      if (first)
        *str = toupper(*str);
      else
        *str = tolower(*str);
    }
    first = 0;
    str += 2;
  }
}

#define __NAME_ENCODE_APPEND(type, item)  \
  *res = type;                            \
  res += 2;                               \
  *(uint16_t *)res = item##_sz - 2;       \
  res += 2;                               \
  memcpy(res, item, item##_sz - 2);       \
  res += item##_sz - 2;                   \

static size_t _ntlm_name_encode(char **names, const char *domain,
                                const char *host, uint64_t ts2)
{
  char    *wdomain, *whost, *wdomain_camel, *whost_camel;
  size_t  wdomain_sz, whost_sz, wdomain_camel_sz, whost_camel_sz;
  char    *res;
  size_t  res_sz;

  assert(names != NULL && domain != NULL && host != NULL);

  wdomain_sz        = smb_to_utf16(domain, strlen(domain) + 1, &wdomain);
  wdomain_camel_sz  = smb_to_utf16(domain, strlen(domain) + 1, &wdomain_camel);
  whost_sz          = smb_to_utf16(host, strlen(host) + 1, &whost);
  whost_camel_sz    = smb_to_utf16(host, strlen(host) + 1, &whost_camel);

  _wcamelcase(wdomain_camel);
  _wcamelcase(whost_camel);

  res_sz = (wdomain_sz - 2) * 2 + (whost_sz - 2) * 2 + 8 + 6 * 4;
  *names = res = malloc(res_sz);
  assert(res != NULL);
  memset(res, 0, res_sz);

  __NAME_ENCODE_APPEND(2, wdomain)
  __NAME_ENCODE_APPEND(1, whost)
  __NAME_ENCODE_APPEND(4, wdomain_camel)
  __NAME_ENCODE_APPEND(3, whost_camel)

  *res = 7;
  res += 2;
  *res = 8;
  res += 2;
  *(uint64_t *)res = ts2;
  res += 8;

  free(wdomain);
  free(wdomain_camel);
  free(whost);
  free(whost_camel);

  return (res_sz);
}

size_t      smb_ntlm_make_blob(smb_ntlm_blob **out_blob, uint64_t ts,
                               uint64_t user_challenge, void *tgt_info,
                               size_t tgt_sz, uint64_t ts2)
{
  smb_ntlm_blob *blob;
  //char          *names;
  //size_t        names_sz;

  assert(blob != NULL && tgt_info != NULL);

  //names_sz = _ntlm_name_encode(&names, domain, host, ts2);
  blob = malloc(tgt_sz + sizeof(smb_ntlm_blob));

  memset((void *)blob, 0, sizeof(smb_ntlm_blob));
  blob->header    = 0x101;
  blob->timestamp = ts;
  blob->challenge = user_challenge;

  memcpy(blob->target, tgt_info, tgt_sz);
  //free(names);

  *out_blob = blob;
  return (sizeof(smb_ntlm_blob) + tgt_sz);
}

void        smb_ntlm2_session_key(smb_ntlmh *hash_v2, void *ntlm2,
                                  smb_ntlmh *xkey, smb_ntlmh *xkey_crypt)
{
  struct rc4_state  rc4;
  smb_ntlmh         hmac_ntlm2;

  HMAC_MD5(&hash_v2, 16, ntlm2, 16, &hmac_ntlm2);

  rc4_init(&rc4, hmac_ntlm2, 16);
  rc4_crypt(&rc4, xkey, xkey_crypt, 16);
}


void        smb_ntlmssp_negotiate(const char *host, const char *domain,
                                  void **token, size_t *token_sz)
{
  smb_ntlmssp_nego  *nego;

  assert(host != NULL && domain != NULL && token != NULL && token_sz != NULL);

  *token_sz = sizeof(smb_ntlmssp_nego) + strlen(host) + strlen(domain);
  if (*token_sz % 2) // Align on Word
    *token_sz += 1;

  // fprintf(stderr, "Token size if %ld\n", *token_sz);

  nego      = (smb_ntlmssp_nego *)malloc(*token_sz);
  assert(nego != NULL);

  nego->type  = SMB_NTLMSSP_CMD_NEGO;
  nego->flags = 0x60088215;//0x20080205;
  nego->domain_len = nego->domain_maxlen = strlen(domain);
  nego->domain_offset = 32;
  nego->host_len = nego->host_maxlen = strlen(host);
  nego->host_offset = 32 + strlen(domain);

  memcpy(nego->id, "NTLMSSP", 8);
  memcpy(nego->names, domain, strlen(domain));
  memcpy(nego->names + strlen(domain), domain, strlen(domain));

  *token = (void *)nego;
}

#define __AUTH_APPEND(FIELD, value, size, cursor)           \
  memcpy(auth->data + cursor, value, size);                 \
  auth-> FIELD ## _len    = auth->FIELD ## _maxlen = size;   \
  auth-> FIELD ## _offset = 64 + cursor;                     \
  cursor += size;

void        smb_ntlmssp_response(uint64_t srv_challenge, uint64_t srv_ts,
                                 const char *host, const char *domain,
                                 const char *user, const char *password,
                                 void *tgt, size_t tgt_sz,
                                 void **token, size_t *token_sz)
{
  smb_ntlmssp_auth      *auth;
  smb_ntlm_blob         *blob;
  smb_ntlmh             hash_v2, xkey, xkey_crypt;
  void                  *lm2, *ntlm2;
  size_t                blob_size, utf_sz, cursor = 0;
  uint64_t              user_challenge;
  char                  *utf;

  assert(host != NULL && domain != NULL && user != NULL && password != NULL);
  assert(token != NULL && token_sz != NULL);

  //// We compute most of the data first to know the final token size
  smb_ntlm2_hash(user, password, domain, &hash_v2);
  user_challenge = smb_ntlm_generate_challenge();
  smb_ntlm_generate_xkey(&xkey);
  blob_size = smb_ntlm_make_blob(&blob, srv_ts, user_challenge, tgt, tgt_sz,
                                 srv_ts + 4200);

  lm2   = smb_lm2_response(&hash_v2, srv_challenge, smb_ntlm_generate_challenge());
  ntlm2 = smb_ntlm2_response(&hash_v2, srv_challenge, blob, blob_size);
  smb_ntlm2_session_key(&hash_v2, ntlm2, &xkey, &xkey_crypt);

  *token_sz = sizeof(smb_ntlmssp_auth)
              + strlen(host) * 2
              + strlen(domain) * 2
              + strlen(user) * 2
              + blob_size + 16 // Blob + HMAC
              + 8 + 16  // LM2 Response (miniblob=user_challenge + HMAC)
              + 16;     // Session Key
  if (*token_sz % 2) // Align on Word
    *token_sz += 1;

  auth = (smb_ntlmssp_auth *)malloc(*token_sz);
  assert(auth != NULL);
  memset(auth, 0, *token_sz);

  memcpy(auth->id, "NTLMSSP", 8);
  auth->type  = SMB_NTLMSSP_CMD_AUTH;
  auth->flags = 0x60088215;


  __AUTH_APPEND(lm, lm2, 24, cursor)
  __AUTH_APPEND(ntlm, ntlm2, blob_size + 16, cursor)

  utf_sz = smb_to_utf16(domain, strlen(domain), &utf);
  __AUTH_APPEND(domain, utf, utf_sz, cursor)
  free(utf);
  utf_sz = smb_to_utf16(user, strlen(user), &utf);
  __AUTH_APPEND(user, utf, utf_sz, cursor)
  free(utf);
  utf_sz = smb_to_utf16(host, strlen(host), &utf);
  __AUTH_APPEND(host, utf, utf_sz, cursor)
  free(utf);

  __AUTH_APPEND(session_key, &xkey_crypt, 16, cursor)

  free(lm2);
  free(ntlm2);

  *token = auth;
}
