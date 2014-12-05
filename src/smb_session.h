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

#ifndef _SMB_SESSION_H_
#define _SMB_SESSION_H_

#include "bdsm/smb_session.h"

/**
 * @file smb_session.h
 * @brief Functions to connect and authenticate to an SMB server
 */

/**
 * @internal
 * @brief Get a smb_tid from a smb_fd
 *
 * @param fd a smb_fd
 * @return A smb_tid
 */
#define SMB_FD_TID(fd)    ((smb_tid)(fd >> 16))
/**
 * @internal
 * @brief Get a smb_fid from a smb_fd
 *
 * @param fd a smb_fid
 * @return A smb_fid
 */
#define SMB_FD_FID(fd)    ((smb_fid)(fd & 0x0000ffff))
/**
 * @internal
 * @brief Compute the smb_fd for the given smb_tid and smb_fid
 * @param tid a smb_tid
 * @param fid a smb_fid *
 * @return A smb_fd
 */
#define SMB_FD(tid, fid)  ((((smb_fd)tid) << 16) | (((smb_fd) fid)))

/* Our reception buffer grows as necessary, so we can put the max here */
#define SMB_SESSION_MAX_BUFFER (0xffff)
 
#endif
