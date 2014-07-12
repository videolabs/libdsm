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
#include <iconv.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_LANGINFO_H && !defined( __APPLE__ )
# include <langinfo.h>
#endif

#include "bdsm/debug.h"
#include "bdsm/smb_utils.h"

static const char *current_encoding()
{
#if defined( __APPLE__ ) || !HAVE_LANGINFO_H
    return ("UTF8");
#else
    static int locale_set = 0;

    if (!locale_set)
    {
        setlocale(LC_ALL, "");
        locale_set = 1;
    }
    //fprintf(stderr, "%s\n", nl_langinfo(CODESET));
    return (nl_langinfo(CODESET));
#endif
}

static size_t smb_iconv(const char *src, size_t src_len, char **dst,
                        size_t dst_len, const char *src_enc,
                        const char *dst_enc)
{
    iconv_t   ic;
    char      *out;
    size_t    outleft;

    assert(src != NULL && dst != NULL && src_enc != NULL && dst_enc != NULL);

    if (!src_len)
    {
        *dst = NULL;
        return (0);
    }

    if ((ic = iconv_open(dst_enc, src_enc)) == (iconv_t)-1)
    {
        fprintf(stderr, "Unable to open iconv to convert from %s to %s\n",
                src_enc, dst_enc);
        return (0);
    }

    outleft = dst_len; // The utf-16 str is at most 2x bigger than the utf-8 one. (i think ?)
    out     = *dst = malloc(outleft);

    assert(out != NULL);
    iconv(ic, (char **)&src, &src_len, &out, &outleft);
    if (src_len > 0)
      BDSM_dbg("iconv: Some character were lost at encoding/decoding");

    return (dst_len - outleft);
}

size_t      smb_to_utf16(const char *src, size_t src_len, char **dst)
{
    return (smb_iconv(src, src_len, dst, src_len * 2,
                      current_encoding(), "UCS-2LE"));
}

size_t      smb_from_utf16(const char *src, size_t src_len, char **dst)
{
    return (smb_iconv(src, src_len, dst, src_len,
                      "UCS-2LE", current_encoding()));
}
