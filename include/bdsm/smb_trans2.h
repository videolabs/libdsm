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

/**
 * @file smb_trans2.h
 * @brief SMB transactions operations: find(i.e. ls), stat
 */

#ifndef __BDSM_SMB_TRANS2_H_
#define __BDSM_SMB_TRANS2_H_

#include "bdsm/smb_defs.h"
#include "bdsm/smb_session.h"

/**
 * @brief Returns infos about files matching a pattern
 * @details This functions uses the FIND_FIRST2 SMB operations to list files
 * matching a certain pattern. It's basically used to list folder contents
 *
 * @param s The session object
 * @param tid The share inside of which we want to find files obtained by
 * smb_tree_connect()
 * @param pattern The pattern to match files. '\\*' will list all the files at
 * the root of the share. '\\afolder\\*' will list all the files inside of the
 * 'afolder' directory.
 * @return A list of stat opaque object containing file status
 */
smb_stat_list_t   smb_find(smb_session_t *s, smb_tid tid, const char *pattern);

/**
 * @brief Get the status of a file from it's path inside of a share
 *
 * @param s The session object
 * @param tid The tree id of a share obtained by smb_tree_connect()
 * @param path The full path of the file relative to the root of the share
 * (e.g. '\\folder\\file.ext')
 * @return An opaque smb_stat_t file representing a file status (informations)
 */
smb_stat_t        smb_stat(smb_session_t *s, smb_tid tid, const char *path);

#endif
