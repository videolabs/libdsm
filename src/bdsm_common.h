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
 * Author: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
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

#ifndef BDSM_COMMON_H
#define BDSM_COMMON_H

#ifdef _MSC_VER
# if defined(__clang__)
#   define SMB_PACKED_START
#   define SMB_PACKED_END    __attribute__((packed))
# else
#   define SMB_PACKED_START  __pragma(pack(push, 1))
#   define SMB_PACKED_END    __pragma(pack(pop))
# endif
#elif defined(__GNUC__)
#  define SMB_PACKED_START
# ifdef _WIN32
#  define SMB_PACKED_END    __attribute__((packed, gcc_struct))
# else
#  define SMB_PACKED_END    __attribute__((packed))
# endif
#endif

#endif // BDSM_COMMON_H
