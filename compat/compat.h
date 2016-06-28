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

#include <stdlib.h>
#if !defined HAVE_STRLCPY && !defined HAVE_LIBBSD
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_CLOCKID_T
typedef int clockid_t;
#endif
#if !HAVE_DECL_CLOCK_MONOTONIC
enum {
    CLOCK_REALTIME,
    CLOCK_MONOTONIC,
    CLOCK_PROCESS_CPUTIME_ID,
    CLOCK_THREAD_CPUTIME_ID
};
#endif
#if !defined HAVE_CLOCK_GETTIME
int clock_gettime(clockid_t clk_id, struct timespec *tp);
#endif

#if !defined HAVE_STRNDUP
char *strndup(const char *str, size_t n);
#endif

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif

#if !defined HAVE_SYS_QUEUE_H
# include "queue.h"
#endif

#if !defined(HAVE_PIPE) && defined(HAVE__PIPE)
#define HAVE_PIPE
int pipe(int fds[2]);
#endif

#ifndef _WIN32
#define closesocket(fd) close(fd)
#endif

#ifndef HAVE_STRUCT_TIMESPEC
struct timespec {
    time_t  tv_sec;   /* Seconds */
    long    tv_nsec;  /* Nanoseconds */
};
#endif /* HAVE_STRUCT_TIMESPEC */
