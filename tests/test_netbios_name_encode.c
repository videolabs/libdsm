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

#include "src/netbios_utils.h"

void test_nb_encode(void **s)
{
  (void) s; // Unused

  const char *name1 = "Lta's Mainframe";
  const char *name1_encoded = "EMFEEBCHFDCAENEBEJEOEGFCEBENEFCA";
  const char *name1_decoded = "LTA'S MAINFRAME";
  const char *name2 = "abcdefghijklmnopqrstuvwxyz";
  const char *name2_decoded = "ABCDEFGHIJKLMNO";
  char encoded[33], decoded[16];

  // Simple
  netbios_name_level1_encode(name1, (char *)encoded, NETBIOS_FILESERVER);
  assert_string_equal(name1_encoded, encoded);
  netbios_name_level1_decode((char *)encoded, (char *)decoded);
  assert_string_equal(name1_decoded, decoded);

  // Truncating at 15 chars
  netbios_name_level1_encode(name2, (char *)encoded, NETBIOS_FILESERVER);
  netbios_name_level1_decode((char *)encoded, (char *)decoded);
  assert_string_equal(name2_decoded, decoded);
}
