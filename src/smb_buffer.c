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

#include <assert.h>
#include <stdlib.h>

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#endif

#include "smb_buffer.h"

void    smb_buffer_init(smb_buffer *buf, void *data, size_t size)
{
    assert(buf != NULL);

    buf->data = data;
    buf->size = size;
}

int     smb_buffer_alloc(smb_buffer *buf, size_t size)
{
    assert(buf != NULL);

    buf->data = malloc(size);
    if (buf->data) {
        buf->size = size;
        return 1;
    } else
        return 0;
}

void    smb_buffer_free(smb_buffer *buf)
{
    if (buf == NULL || buf->data == NULL)
        return;

    free(buf->data);
    smb_buffer_init(buf, NULL, 0);
}
