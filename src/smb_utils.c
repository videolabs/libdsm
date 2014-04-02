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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bdsm/smb_utils.h"

size_t      smb_to_utf16(const char *src_enc, const char *src,
                         size_t src_len, char **dst)
{
  size_t    res = src_len * 2;
  char      *out;

  out = malloc(res);
  assert(out != NULL);
  *dst = (char *)out;

  for (size_t i = 0; i < src_len; i++)
  {
    out[2 * i]      = src[i];
    out[2 * i + 1]  = 0;
  }

  return (res);

}
