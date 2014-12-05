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

#ifndef _BDSM_DEBUG_H_
# define _BDSM_DEBUG_H_

# include "config.h"

# ifdef __ANDROID__
#  define LOG_TAG "libdsm"
#  include <android/log.h>
#  include <stdio.h>
#  include <errno.h>
#  include <stdarg.h>
   static inline void BDSM_perror(const char *format, ...)
   {
       va_list ap;
       char *buf;

       va_start(ap, format);
       if (vasprintf(&buf, format, ap) == -1) {
           va_end(ap);
           return -1;
       }
       va_end(ap);
       __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s%s",
                           buf, strerror(errno));
       free(buf);
   }
# else
#   include <stdio.h>
#   define BDSM_perror(...) perror(__VA_ARGS__)
# endif

# ifdef BDSM_DEBUG
#  ifdef __ANDROID__
#   define BDSM_dbg(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#  else
#   define BDSM_dbg(...) fprintf(stderr, __VA_ARGS__)
#  endif
# else
#  define BDSM_dbg(...) do { } while(0)
# endif

#endif
