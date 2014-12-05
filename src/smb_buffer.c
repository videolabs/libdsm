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

#include <alloca.h>
#include <assert.h>
#include <stdlib.h>

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
        return (1);
    } else
        return (0);
}

int     smb_buffer_alloca(smb_buffer *buf, size_t size)
{
    assert(buf != NULL);

    buf->data = alloca(size);
    if (buf->data) {
        buf->size = size;
        return (1);
    } else
        return (0);
}

void    smb_buffer_free(smb_buffer *buf)
{
    if (buf == NULL || buf->data == NULL)
        return;

    free(buf->data);
    smb_buffer_init(buf, NULL, 0);
}
