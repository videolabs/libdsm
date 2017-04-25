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

#include "src/smb_buffer.h"

void test_smb_buffer_init(void **s)
{
  (void) s; // Unused

  void *data;
  smb_buffer buf;

  data = malloc(42);
  smb_buffer_init(&buf, data, 42);
  assert_ptr_equal(buf.data, data);
  assert_true(buf.size == 42);

  smb_buffer_free(&buf);
  assert_null(buf.data);
  assert_true(buf.size == 0);
}

void test_smb_buffer_alloc(void **s)
{
  (void) s; // Unused

  smb_buffer buf;

  smb_buffer_alloc(&buf, 42);
  assert_non_null(buf.data);
  assert_true(buf.size == 42);

  // Triggers a segfault is alloc is faulty
  ((char *)buf.data)[0] + 42;

  smb_buffer_free(&buf);
  assert_null(buf.data);
  assert_true(buf.size == 0);
}
