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
#include <stdlib.h>

#include "tests.h"

#include "src/smb_utils.h"

void test_smb_utf16(void **s)
{
  (void) s; // Unused

  const char str1[] = "only ascii chars";
  const char str2[] = "àéïóù";
  size_t result_size1, result_size2;
  char *result1, *result2;

  // Only ASCII
  result_size1 = smb_to_utf16(str1, strlen(str1), &result1);
  assert_true(result_size1 == strlen(str1) * 2);

  result_size2 = smb_from_utf16(result1, result_size1, &result2);
  assert_memory_equal(str1, result2, result_size2);

  free(result1);
  free(result2);

  // With Noice accents
  result_size1 = smb_to_utf16(str2, strlen(str2), &result1);
  result_size2 = smb_from_utf16(result1, result_size1, &result2);
  assert_memory_equal(str2, result2, result_size2);

  free(result1);
  free(result2);


}
