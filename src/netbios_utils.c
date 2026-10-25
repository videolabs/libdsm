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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifndef _WIN32
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <poll.h>
#include <sys/time.h>
#endif

#include "netbios_utils.h"

#ifdef NS_ABORT_USE_PIPE
#include <unistd.h>
#include <fcntl.h>
#endif

static short  nibble_encode(char c)
{
    short n1, n2;

    n1 = (toupper(c) >> 4)   + 'A';
    n2 = (toupper(c) & 0x0F) + 'A';

    return ((n1 << 8) | n2);
}

static char   nibble_decode(char c1, char c2)
{
    return (((c1 - 'A') << 4) + (c2 - 'A'));
}


// name must be null-terminated, it will be truncated if >= 16 chars
// encoded name must the 33bytes long at least, will be null-terminated
void  netbios_name_level1_encode(const char *name, char *encoded_name,
                                 unsigned type)
{
    size_t    name_length = strlen(name);

    if (name_length > NETBIOS_NAME_LENGTH)
        name_length = NETBIOS_NAME_LENGTH;

    for (unsigned int i = 0; i < NETBIOS_NAME_LENGTH; i++)
    {
        if (i < name_length)
        {
            encoded_name[2 * i]     = nibble_encode(name[i]) >> 8;
            encoded_name[2 * i + 1] = nibble_encode(name[i]) & 0x00FF;
        }
        else
        {
            encoded_name[2 * i]     = 'C';
            encoded_name[2 * i + 1] = 'A';
        }
    }

    encoded_name[30] = nibble_encode(type) >> 8;
    encoded_name[31] = nibble_encode(type) &  0x00FF;
    encoded_name[32] = '\0';
}

// encoded_name: 33bytes long
// name: at least 16bytes long (the string will be null-terminated)
void  netbios_name_level1_decode(const char *encoded_name, char *name)
{
    for (unsigned int i = 0; i < NETBIOS_NAME_LENGTH; i++)
    {
        name[i] = nibble_decode(encoded_name[2 * i], encoded_name[2 * i + 1]);
    }
    name[NETBIOS_NAME_LENGTH] = 0;
}

// XXX: Supports domain
char  *netbios_name_encode(const char *name, char *domain,
                           unsigned type)
{
    (void)domain;  // Unused yet

    size_t    encoded_size = 34; // length byte + 32 bytes for encoded name + terminator
    char      *encoded_name;

    if (!name)
        return NULL;

    encoded_name = malloc(encoded_size);
    if (!encoded_name)
        return NULL;
    encoded_name[0] = 32; // length of the field;
    netbios_name_level1_encode(name, encoded_name + 1, type);
    encoded_name[33] = 0;

    //printf("Encoded name (l2): %s.\n", encoded_name);

    return encoded_name;
}

int             netbios_name_decode(const char *encoded_name,
                                    char *name, char **domain)
{
    (void)domain;  // Unused yet

    size_t  encoded_length;

    if (!encoded_name || !name)
        return -1;

    encoded_length =  strlen(encoded_name);

    // XXX Support domain name :p
    if (encoded_length != 33)
        return -1;

    netbios_name_level1_decode(encoded_name + 1, name);
    name[33] = '\0';
    return 32;
}

#ifdef NS_ABORT_USE_PIPE

void netbios_abort_ctx_destroy(struct netbios_abort_ctx *ctx)
{
    if (ctx->pipe[0] != -1 && ctx->pipe[1] != -1)
    {
        close(ctx->pipe[0]);
        close(ctx->pipe[1]);
        ctx->pipe[0] = ctx->pipe[1] = -1;
    }
}

int netbios_abort_ctx_init(struct netbios_abort_ctx *ctx)
{
    int flags;

    if (pipe(ctx->pipe) == -1)
        return -1;

    if ((flags = fcntl(ctx->pipe[0], F_GETFL, 0)) == -1)
    {
        netbios_abort_ctx_destroy(ctx);
        return -1;
    }

    if (fcntl(ctx->pipe[0], F_SETFL, flags | O_NONBLOCK) == -1)
    {
        netbios_abort_ctx_destroy(ctx);
        return -1;
    }

    return 0;
}

bool netbios_abort_ctx_is_aborted(struct netbios_abort_ctx *ctx)
{
    struct pollfd fd = { .fd = ctx->pipe[0], .events = POLLIN };
    int res = poll(&fd, 1, 0);
    return res < 0 || fd.revents != 0;
}

void netbios_abort_ctx_abort(struct netbios_abort_ctx *ctx)
{
    uint8_t buf = '\0';
    ssize_t ret = write(ctx->pipe[1], &buf, sizeof(uint8_t));
    (void) ret;
}

#else

int netbios_abort_ctx_init(struct netbios_abort_ctx *ctx)
{
    atomic_init(&ctx->aborted, false);
    return 0;
}

void netbios_abort_ctx_destroy(struct netbios_abort_ctx *ctx)
{
    (void) ctx;
}

bool netbios_abort_ctx_is_aborted(struct netbios_abort_ctx *ctx)
{
    return atomic_load(&ctx->aborted);
}

void netbios_abort_ctx_abort(struct netbios_abort_ctx *ctx)
{
    atomic_store(&ctx->aborted, true);
}

#endif

#ifndef _WIN32
int netbios_wait(struct netbios_abort_ctx *ctx, int fd, bool writing,
                 struct timeval *timeout)
{
    if (fd < 0)
    {
        errno = EBADF;
        return -1;
    }
    struct pollfd fds[2] = {
        { .fd = fd, .events = writing ? POLLOUT : POLLIN },
        { .fd = -1, .events = POLLIN },
    };
#ifdef NS_ABORT_USE_PIPE
    fds[1].fd = ctx->pipe[0];
#else
    if (netbios_abort_ctx_is_aborted(ctx))
    {
        errno = EINTR;
        return -1;
    }
#endif
    int milliseconds = -1;
    int64_t remaining = 0;
    struct timespec before;
    if (timeout != NULL)
    {
        remaining = (int64_t)timeout->tv_sec * 1000000 + timeout->tv_usec;
        int64_t rounded = (remaining + 999) / 1000;
        milliseconds = rounded > INT_MAX ? INT_MAX : (int)rounded;
        if (clock_gettime(CLOCK_MONOTONIC, &before) != 0)
            return -1;
    }
    int res = poll(fds, 2, milliseconds);
    int saved_errno = errno;
    if (timeout != NULL)
    {
        struct timespec after;
        if (clock_gettime(CLOCK_MONOTONIC, &after) != 0)
            return -1;
        int64_t elapsed = (int64_t)(after.tv_sec - before.tv_sec) * 1000000
                         + (after.tv_nsec - before.tv_nsec) / 1000;
        remaining = elapsed >= remaining ? 0 : remaining - elapsed;
        timeout->tv_sec = remaining / 1000000;
        timeout->tv_usec = remaining % 1000000;
    }
    errno = saved_errno;
    if (res < 0)
        return -1;
#ifdef NS_ABORT_USE_PIPE
    if (fds[1].revents != 0)
#else
    if (netbios_abort_ctx_is_aborted(ctx))
#endif
    {
        errno = EINTR;
        return -1;
    }
    if (fds[0].revents & POLLNVAL)
    {
        errno = EBADF;
        return -1;
    }
    /* Let the subsequent send/recv/getsockopt report EOF or socket errors. */
    return fds[0].revents != 0 ? 1 : 0;
}
#endif
