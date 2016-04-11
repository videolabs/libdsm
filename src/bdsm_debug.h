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
           return;
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

# ifndef NDEBUG
#  ifdef __ANDROID__
#   define BDSM_dbg(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#  else
#   define BDSM_dbg(...) fprintf(stderr, __VA_ARGS__)
#  endif
# else
#  define BDSM_dbg(...) do { } while(0)
# endif

#endif
