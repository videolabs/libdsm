/*
 * Copyright (c), MM Weiss
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the MM Weiss nor the names of its contributors
 *     may be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _WIN32

#include <sys/time.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_time.h>
#include <unistd.h>
#include <sched.h>
#include "compat.h"

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    kern_return_t   ret;
    clock_serv_t    clk;
    clock_id_t clk_serv_id;
    mach_timespec_t tm;

    if (clk_id != CLOCK_REALTIME)
        return -1;

    clk_serv_id = clk_id == CLOCK_REALTIME ? CALENDAR_CLOCK : SYSTEM_CLOCK;
    if (KERN_SUCCESS == (ret = host_get_clock_service(mach_host_self(), clk_serv_id, &clk))) {
        if (KERN_SUCCESS == (ret = clock_get_time(clk, &tm))) {
            tp->tv_sec  = tm.tv_sec;
            tp->tv_nsec = tm.tv_nsec;
        }
    }
    if (KERN_SUCCESS != ret)
        return -1;

    return 0;
}

#else

#include <windows.h>
#include <stdint.h>
#include <time.h>
#include "compat.h"

int clock_gettime(int clk_id, struct timespec *spec) {
    (void)clk_id;
    FILETIME wintime;
    GetSystemTimeAsFileTime(&wintime);
    uint64_t t = (uint64_t)wintime.dwHighDateTime << 32 | wintime.dwLowDateTime;
    t -= 116444736000000000;
    spec->tv_sec  = t / 10000000;
    spec->tv_nsec = t % 10000000 * 100;
    return 0;
}
#endif
