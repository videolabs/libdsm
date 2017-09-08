/*****************************************************************************
 *  __________________    _________  _____            _____  .__         ._.
 *  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
 *   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
 *   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
 *   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
 *          \/        \/        \/        \/  )/           \/        \/   \/
 *
 * This file is part of liBDSM. Copyright © 2014-2017 VideoLabs SAS
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

#include <string.h>

#include "tests.h"

#include "src/hmac_md5.h"
#include "src/smb_ntlm.h"

void test_hmac_md5(void **s)
{
  (void)s;

  const char key1[] = "12345678";
  const char key2[] = "aaaabbbb";
  const char msg1[] = "'Wut wut' is first message :)";
  const char msg2[] = "A second awesome message !";
  smb_ntlmh hash1, hash2;

  // Hashing the same things should give the same result
  HMAC_MD5(key1, strlen(key1), msg1, strlen(msg1), hash1);
  HMAC_MD5(key1, strlen(key1), msg1, strlen(msg1), hash2);
  assert_memory_equal(hash1, hash2, sizeof(hash1));

  // Hashing different stuff gives different results
  HMAC_MD5(key2, strlen(key2), msg2, strlen(msg2), hash2);
  assert_memory_not_equal(hash1, hash2, sizeof(hash1));

  // Test against a precomputed hmac
  smb_ntlmh expected_hash1 = { 0xc7, 0x30, 0x7e, 0x75, 0x1b, 0x42, 0xb9, 0x37,
                               0xc8, 0x01, 0x22, 0xe2, 0x09, 0xda, 0x75, 0x0a };
  assert_memory_equal(hash1, expected_hash1, sizeof(hash1));
}
