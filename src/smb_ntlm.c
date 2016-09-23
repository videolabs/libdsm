/*****************************************************************************
 *  __________________    _________  _____            _____  .__         ._.
 *  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
 *   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
 *   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
 *   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
 *          \/        \/        \/        \/  )/           \/        \/   \/
 *
 * This file is part of liBDSM. Copyright © 2014-2015 VideoLabs SAS
 *
 * Author: Julien 'Lta' BALLET <contact@lta.io>
 *
 * liBDSM is released under LGPLv2.1 (or later) and is also available
 * under a commercial license.
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "config.h"

#ifdef _WIN32
# define _CRT_RAND_S
#endif

#include <assert.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HAVE_BSD_STRING_H
#include <bsd/string.h>
#else
#include "compat.h"
#endif

#include "mdx/md4.h"
#include "rc4/rc4.h"
#include "bdsm_debug.h"
#include "hmac_md5.h"
#include "smb_utils.h"
#include "smb_ntlm.h"

uint64_t    smb_ntlm_generate_challenge()
{
#if !defined(_WIN32)
    uint64_t        result;
    int             fd;

    fd = open(URANDOM, O_RDONLY);
    if (fd >= 0)
    {
        while(read(fd, (void *)&result, sizeof(result)) != sizeof(result))
            ;

        closesocket(fd);
        return result;
    }
    else
    {
        /* FIXME: Wrong on a arch with long is 32 bits */
        return random();
    }
#else
    unsigned int number;
    rand_s( &number );
    return number;
#endif
}

void        smb_ntlm_generate_xkey(smb_ntlmh cli_session_key)
{
    uint64_t  *key = (uint64_t *)cli_session_key;

    key[0] = smb_ntlm_generate_challenge();
    key[1] = smb_ntlm_generate_challenge();
}

void        smb_ntlm_hash(const char *password, smb_ntlmh hash)
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
    assert(str != NULL);

    while (*str)
    {
        if (isalpha(*str))
            *str = toupper(*str);
        str++;
    }
}

void        smb_ntlm2_hash(const char *user, const char *password,
                           const char *dest, smb_ntlmh hash)
{
    smb_ntlmh     hash_v1;
    char          *ucs_user, *ucs_dest, *data, user_upper[64];
    size_t        ucs_user_len, ucs_dest_len, data_len;

    smb_ntlm_hash(password, hash_v1);

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

uint8_t     *smb_ntlm2_response(smb_ntlmh hash_v2, uint64_t srv_challenge,
                                smb_buffer *blob)
{
    smb_buffer      data;
    uint8_t         *response, hmac[16];


    if (smb_buffer_alloc(&data, sizeof(uint64_t) + blob->size) == 0)
        return NULL;
    memcpy(data.data, (void *)&srv_challenge, sizeof(uint64_t));
    memcpy((uint8_t*)data.data + sizeof(uint64_t), blob->data, blob->size);

    HMAC_MD5(hash_v2, SMB_NTLM_HASH_SIZE, data.data, data.size, &hmac);
    smb_buffer_free(&data);

    response = malloc(blob->size + 16);
    if (!response)
        return NULL;

    memcpy(response, (void *)hmac, 16);
    memcpy(response + 16, blob->data, blob->size);

    return response;
}

uint8_t     *smb_lm2_response(smb_ntlmh hash_v2, uint64_t srv_challenge,
                              uint64_t user_challenge)
{
    smb_buffer buf;

    smb_buffer_init(&buf, (void *)&user_challenge, 8);
    return smb_ntlm2_response(hash_v2, srv_challenge, &buf);
}

// static void   _wcamelcase(char *str)
// {
//   int first = 1;

//   assert (str != NULL);

//   while(*str)
//   {
//     if (isalpha(*str))
//     {
//       if (first)
//         *str = toupper(*str);
//       else
//         *str = tolower(*str);
//     }
//     first = 0;
//     str += 2;
//   }
// }

// // This was test code to encode the name the way the server looked to expect
// // it. But apparently we just had to sent him back his own data.

/* #define __NAME_ENCODE_APPEND(type, item)  \
   *res = type;                            \
   res += 2;                               \
   *(uint16_t *)res = item##_sz - 2;       \
   res += 2;                               \
   memcpy(res, item, item##_sz - 2);       \
   res += item##_sz - 2;                   \
*/
// static size_t _ntlm_name_encode(char **names, const char *domain,
//                                 const char *host, uint64_t ts2)
// {
//   char    *wdomain, *whost, *wdomain_camel, *whost_camel;
//   size_t  wdomain_sz, whost_sz, wdomain_camel_sz, whost_camel_sz;
//   char    *res;
//   size_t  res_sz;

//   assert(names != NULL && domain != NULL && host != NULL);

//   wdomain_sz        = smb_to_utf16(domain, strlen(domain) + 1, &wdomain);
//   wdomain_camel_sz  = smb_to_utf16(domain, strlen(domain) + 1, &wdomain_camel);
//   whost_sz          = smb_to_utf16(host, strlen(host) + 1, &whost);
//   whost_camel_sz    = smb_to_utf16(host, strlen(host) + 1, &whost_camel);

//   _wcamelcase(wdomain_camel);
//   _wcamelcase(whost_camel);

//   res_sz = (wdomain_sz - 2) * 2 + (whost_sz - 2) * 2 + 8 + 6 * 4;
//   *names = res = malloc(res_sz);
//   assert(res != NULL);
//   memset(res, 0, res_sz);

//   __NAME_ENCODE_APPEND(2, wdomain)
//   __NAME_ENCODE_APPEND(1, whost)
//   __NAME_ENCODE_APPEND(4, wdomain_camel)
//   __NAME_ENCODE_APPEND(3, whost_camel)

//   *res = 7;
//   res += 2;
//   *res = 8;
//   res += 2;
//   *(uint64_t *)res = ts2;
//   res += 8;

//   free(wdomain);
//   free(wdomain_camel);
//   free(whost);
//   free(whost_camel);

//   return res_sz;
// }

size_t      smb_ntlm_make_blob(smb_ntlm_blob **out_blob, uint64_t ts,
                               uint64_t user_challenge, smb_buffer *target)
{
    smb_ntlm_blob *blob;

    assert(out_blob != NULL && target != NULL);

    blob = malloc(target->size + sizeof(smb_ntlm_blob));
    if (!blob)
        return 0;

    memset((void *)blob, 0, sizeof(smb_ntlm_blob));
    blob->header    = 0x101;
    blob->timestamp = ts;
    blob->challenge = user_challenge;

    memcpy(blob->target, target->data, target->size);

    *out_blob = blob;
    return sizeof(smb_ntlm_blob) + target->size;
}

void        smb_ntlm2_session_key(smb_ntlmh hash_v2, void *ntlm2,
                                  smb_ntlmh xkey, smb_ntlmh xkey_crypt)
{
    struct rc4_state  rc4;
    smb_ntlmh         hmac_ntlm2;

    HMAC_MD5(hash_v2, SMB_NTLM_HASH_SIZE, ntlm2, SMB_NTLM_HASH_SIZE, hmac_ntlm2);

    rc4_init(&rc4, hmac_ntlm2, 16);
    rc4_crypt(&rc4, (void *)xkey, (void *)xkey_crypt, 16);
}


void        smb_ntlmssp_negotiate(const char *host, const char *domain,
                                  smb_buffer *token)
{
    smb_ntlmssp_nego  *nego;

    assert(host != NULL && domain != NULL && token != NULL);

    token->size = sizeof(smb_ntlmssp_nego) + strlen(host) + strlen(domain);
    if (token->size % 2) // Align on Word
        token->size += 1;
    if (smb_buffer_alloc(token, token->size) == 0)
        return;
    // BDSM_dbg("Token size if %ld\n", token->size);


    nego      = (smb_ntlmssp_nego *)token->data;

    nego->type  = SMB_NTLMSSP_CMD_NEGO;
    nego->flags = 0x60088215;//0x20080205;
    nego->domain_len = nego->domain_maxlen = strlen(domain);
    nego->domain_offset = 32;
    nego->host_len = nego->host_maxlen = strlen(host);
    nego->host_offset = 32 + strlen(domain);

    memcpy(nego->id, "NTLMSSP", 8);
    memcpy(nego->names, domain, strlen(domain));
    memcpy(nego->names + strlen(domain), domain, strlen(domain));
}

#define __AUTH_APPEND(FIELD, value, size, cursor)           \
  memcpy(auth->data + cursor, value, size);                 \
  auth-> FIELD ## _len    = auth->FIELD ## _maxlen = size;   \
  auth-> FIELD ## _offset = 64 + cursor;                     \
  cursor += size;

void        smb_ntlmssp_response(uint64_t srv_challenge, uint64_t srv_ts,
                                 const char *host, const char *domain,
                                 const char *user, const char *password,
                                 smb_buffer *target, smb_buffer *token)
{
    smb_ntlmssp_auth      *auth;
    smb_ntlm_blob         *blob = NULL;
    smb_ntlmh             hash_v2, xkey, xkey_crypt;
    smb_buffer            buf;
    void                  *lm2, *ntlm2;
    size_t                blob_size, utf_sz, cursor = 0;
    uint64_t              user_challenge;
    char                  *utf;

    assert(host != NULL && domain != NULL && user != NULL && password != NULL);
    assert(token != NULL && target != NULL);

    //// We compute most of the data first to know the final token size
    smb_ntlm2_hash(user, password, domain, hash_v2);
    user_challenge = smb_ntlm_generate_challenge();
    smb_ntlm_generate_xkey(xkey);
    blob_size = smb_ntlm_make_blob(&blob, srv_ts, user_challenge, target);

    lm2   = smb_lm2_response(hash_v2, srv_challenge, smb_ntlm_generate_challenge());
    smb_buffer_init(&buf, blob, blob_size);
    ntlm2 = smb_ntlm2_response(hash_v2, srv_challenge, &buf);
    smb_ntlm2_session_key(hash_v2, ntlm2, xkey, xkey_crypt);

    smb_buffer_init(&buf, NULL, 0);
    free(blob);

    // Compute size of and allocate token
    token->size = sizeof(smb_ntlmssp_auth)
                  + strlen(host) * 2
                  + strlen(domain) * 2
                  + strlen(user) * 2
                  + blob_size + 16 // Blob + HMAC
                  + 8 + 16  // LM2 Response (miniblob=user_challenge + HMAC)
                  + 16;     // Session Key
    if (token->size % 2) // Align on Word
        token->size += 1;
    if (smb_buffer_alloc(token, token->size) == 0) {
        free(lm2);
        free(ntlm2);
        return;
    }

    auth = (smb_ntlmssp_auth *)token->data;
    memset(auth, 0, token->size);

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
}
