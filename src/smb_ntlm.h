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

#ifndef _SMB_NTLM_H_
#define _SMB_NTLM_H_

#include "bdsm_common.h"
#include "smb_defs.h"
#include "smb_buffer.h"

#define SMB_LM2_BLOB_SIZE         8
#define SMB_NTLM_HASH_SIZE        16

typedef uint8_t smb_ntlmh[SMB_NTLM_HASH_SIZE];

SMB_PACKED_START typedef struct
{
    uint32_t    header;
    uint32_t    reserved;
    uint64_t    timestamp;
    uint64_t    challenge;
    uint32_t    unknown;
    uint8_t     target[];
} SMB_PACKED_END smb_ntlm_blob;



#define SMB_NTLMSSP_CMD_NEGO      0x01
#define SMB_NTLMSSP_CMD_AUTH      0x03

#define _NTLMSSP_COMMON     \
  char        id[8];        \
  uint32_t    type;

#define _NTLMSSP_FIELD(FIELD)    \
  uint16_t  FIELD ## _len;       \
  uint16_t  FIELD ## _maxlen;    \
  uint32_t  FIELD ## _offset;

SMB_PACKED_START typedef struct
{
    _NTLMSSP_COMMON
    uint32_t    flags;
    _NTLMSSP_FIELD(domain)
    _NTLMSSP_FIELD(host)
    uint8_t     names[];
} SMB_PACKED_END smb_ntlmssp_nego;

SMB_PACKED_START typedef struct
{
    _NTLMSSP_COMMON
    _NTLMSSP_FIELD(name)
    uint32_t    flags;
    uint64_t    challenge;
    uint64_t    reserved;
    _NTLMSSP_FIELD(tgt) // Target Info
    uint8_t     data[];
} SMB_PACKED_END smb_ntlmssp_challenge;

SMB_PACKED_START typedef struct
{
    _NTLMSSP_COMMON
    _NTLMSSP_FIELD(lm)
    _NTLMSSP_FIELD(ntlm)
    _NTLMSSP_FIELD(domain)
    _NTLMSSP_FIELD(user)
    _NTLMSSP_FIELD(host)
    _NTLMSSP_FIELD(session_key)

    uint32_t    flags;
    uint8_t     data[];
} SMB_PACKED_END smb_ntlmssp_auth;

uint64_t    smb_ntlm_generate_challenge();
void        smb_ntlm_generate_xkey(smb_ntlmh cli_session_key);
void        smb_ntlm_hash(const char *password, smb_ntlmh hash);
void        smb_ntlm2_hash(const char *username, const char *password,
                           const char *destination, smb_ntlmh hash);
// Precompute the blob that will be HMAC'ed to produce NTLM2 Response
// You have to free() the blob after usage
size_t      smb_ntlm_make_blob(smb_ntlm_blob **blob, uint64_t ts,
                               uint64_t user_challenge, smb_buffer *target);
// Returned response is blob_size + 16 long. You'll have to free it
uint8_t     *smb_ntlm2_response(smb_ntlmh hash_v2, uint64_t srv_challenge,
                                smb_buffer *blob);
// Returned response is 24 bytes long. You'll have to free it.
uint8_t     *smb_lm2_response(smb_ntlmh hash_v2, uint64_t srv_challenge,
                              uint64_t user_challenge);
// You have to allocate session key
void        smb_ntlm2_session_key(smb_ntlmh hash_v2, void *ntlm2,
                                  smb_ntlmh xkey, smb_ntlmh enc_xkey);

void        smb_ntlmssp_negotiate(const char *host, const char *domain,
                                  smb_buffer *token);
void        smb_ntlmssp_response(uint64_t srv_challenge, uint64_t srv_ts,
                                 const char *host, const char *domain,
                                 const char *user, const char *password,
                                 smb_buffer *target, smb_buffer *token);

#endif
