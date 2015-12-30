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

/**
 * @file smb_defs.h
 * @brief SMB usefull constants
 */

#ifndef _SMB_DEFS_H_
#define _SMB_DEFS_H_

#include "bdsm/smb_defs.h"

//-----------------------------------------------------------------------------/
// Our own identity
//-----------------------------------------------------------------------------/
#define SMB_OS                  "Unix"
#define SMB_LANMAN              "liBDSM"


//-----------------------------------------------------------------------------/
// A few magic stuffs (SMB magic // our dialect support)
//-----------------------------------------------------------------------------/
#define SMB_MAGIC               { 0xff, 0x53, 0x4d, 0x42 } // aka "\xffSMB"
#define SMB_DIALECTS {          \
  "\2Samba",                    \
  "\2NT LM 0.12",               \
  NULL                          \
}
// Dialect values must match position on SMB_DIALECTS array
#define SMB_DIALECT_SAMBA       0
#define SMB_DIALECT_NTLM        1


//-----------------------------------------------------------------------------/
// SMB Operations/Commands
//-----------------------------------------------------------------------------/
#define SMB_CMD_CLOSE           0x04
#define SMD_CMD_TRANS           0x25
#define SMB_CMD_TRANS2          0x32
#define SMB_CMD_TREE_DISCONNECT 0x71
#define SMB_CMD_NEGOTIATE       0x72
#define SMB_CMD_SETUP           0x73 // Session Setup AndX
#define SMB_CMD_TREE_CONNECT    0x75 // Tree Connect AndX
#define SMB_CMD_ECHO            0x2b
#define SMB_CMD_READ            0x2e // Read AndX
#define SMB_CMD_WRITE           0x2f // Write AndX
#define SMB_CMD_CREATE          0xa2 // NT Create AndX
#define SMB_CMD_MKDIR           0x00 // Depecated
#define SMB_CMD_RMDIR           0x01
#define SMB_CMD_RMFILE          0x06
#define SMB_CMD_MOVE            0x07 // Move or rename

//-----------------------------------------------------------------------------/
// SMB FLAGS2 values
//-----------------------------------------------------------------------------/
#define SMB_FLAGS2_SHORT_NAMES  0x0000
#define SMB_FLAGS2_LONG_NAMES   0x0001

//-----------------------------------------------------------------------------/
// SMB TRANS2 SubCommands
//-----------------------------------------------------------------------------/
#define SMB_TR2_FIND_FIRST        0x0001
#define SMB_TR2_FIND_NEXT         0x0002
#define SMB_TR2_QUERY_PATH        0x0005
#define SMB_TR2_CREATE_DIRECTORY  0x000d


//-----------------------------------------------------------------------------/
// SMB TRANS2 FIND interest values
//-----------------------------------------------------------------------------/
#define SMB_FIND2_INTEREST_DIRECTORY_INFO      0x0101
#define SMB_FIND2_INTEREST_FULL_DIRECTORY_INFO 0x0102
#define SMB_FIND2_INTEREST_NAMES_INFO          0x0103
#define SMB_FIND2_INTEREST_BOTH_DIRECTORY_INFO 0x0104


//-----------------------------------------------------------------------------/
// SMB TRANS2 QUERY (FILE & PATH) interest values
//-----------------------------------------------------------------------------/
#define SMB_FIND2_QUERY_FILE_BASIC_INFO       0x0101
#define SMB_FIND2_QUERY_FILE_STANDARD_INFO    0x0102
#define SMB_FIND2_QUERY_FILE_EA_INFO          0x0103
#define SMB_FIND2_QUERY_FILE_NAME_INFO        0x0104
#define SMB_FIND2_QUERY_FILE_ALL_INFO         0x0107
#define SMB_FIND2_QUERY_FILE_ALT_NAME_INFO    0x0108
#define SMB_FIND2_QUERY_FILE_STREAM_INFO      0x0109
#define SMB_FIND2_QUERY_FILE_COMPRESSION_INFO 0x010B

//-----------------------------------------------------------------------------/
// SMB CMD CREATE Impersonation level values
//-----------------------------------------------------------------------------/
#define SMB_IMPERSONATION_SEC_ANONYMOUS       0
#define SMB_IMPERSONATION_SEC_IDENTIFY        1
#define SMB_IMPERSONATION_SEC_IMPERSONATE     2


///////////////////////////////////////////////////////////////////////////////
//// Flags definitions
//// Many aren't use in libdsm but are here for possible later use

// Protocol negotiation flags (flags field in spec)
#define SMB_FLAG_RESPONSE       (1 << 7)
#define SMB_FLAG_NOTIFY         (1 << 6)
#define SMB_FLAG_OPLOCK         (1 << 5)
#define SMB_FLAG_CANONIC        (1 << 4)
#define SMB_FLAG_CASELESS       (1 << 3)
#define SMB_FLAG_BUFFER_POSTED  (1 << 1)
#define SMB_FLAG_LOCK_AND_READ  (1 << 0)
// More Protocol negotiation flags (flags2 field in spec)
#define SMB_FLAG_UNICODE        (1 << (15 + 8))
#define SMB_FLAG_NT_ERRORS      (1 << (14 + 8))
#define SMB_FLAG_EXECUTE_ONLY   (1 << (13 + 8))
#define SMB_FLAG_DFS            (1 << (12 + 8))
#define SMB_FLAG_XSEC           (1 << (11 + 8))
#define SMB_FLAG_REPARSE_PATH   (1 << (10 + 8))
#define SMB_FLAG_LONG_NAMES     (1 << (6 + 8))
#define SMB_FLAG_SIGN_REQUIRED  (1 << (4 + 8))
#define SMB_FLAG_COMPRESSED     (1 << (3 + 8))
#define SMB_FLAG_SIGN_SUPPORT   (1 << (2 + 8))
#define SMB_FLAG_EXT_ATTR       (1 << (1 + 8))
#define SMB_FLAG_LONG_NAMES_OK  (1 << (0 + 8))

// Negotiated server capabilities
#define SMB_CAPS_RAW            (1 << 0)
#define SMB_CAPS_MPX            (1 << 1)
#define SMB_CAPS_UNICODE        (1 << 2)
#define SMB_CAPS_LARGE          (1 << 3)
#define SMB_CAPS_NTSMB          (1 << 4)
#define SMB_CAPS_RPC            (1 << 5)
#define SMB_CAPS_NTFIND         (1 << 9)
#define SMB_CAPS_XSEC           (1 << 31)

// File creation/open flags
#define SMB_CREATE_OPLOCK       (1 << 1)
#define SMB_CREATE_BATCH_OPLOCK (1 << 2)
#define SMB_CREATE_MKDIR        (1 << 3)
#define SMB_CREATE_EXT_RESP     (1 << 4)
#define SMB_CREATE_DEFAULTS     (0)

// File attributes
#define SMB_ATTR_NORMAL         0
#define SMB_ATTR_RO             (1 << 0)
#define SMB_ATTR_HIDDEN         (1 << 1)
#define SMB_ATTR_SYS            (1 << 2)
#define SMB_ATTR_VOLID          (1 << 3)  // Volume ID
#define SMB_ATTR_DIR            (1 << 4)
#define SMB_ATTR_ARCHIVE        (1 << 5)  // Modified since last archive (!?)

// Share access flags
#define SMB_SHARE_READ          (1 << 0)
#define SMB_SHARE_WRITE         (1 << 1)
#define SMB_SHARE_DELETE        (1 << 2)

// Create disposition flags
#define SMB_DISPOSITION_FILE_SUPERSEDE      0
#define SMB_DISPOSITION_FILE_OPEN           (1 << 0)
#define SMB_DISPOSITION_FILE_CREATE         (1 << 1)
#define SMB_DISPOSITION_FILE_OPEN_IF        (1 << 2)
#define SMB_DISPOSITION_FILE_OVERWRITE      (1 << 3)
#define SMB_DISPOSITION_FILE_OVERWRITE_IF   (1 << 4)

// Create options flags
#define SMB_CREATEOPT_DIRECTORY_FILE             (1 << 0)
#define SMB_CREATEOPT_WRITE_THROUGH              (1 << 1)
#define SMB_CREATEOPT_SEQUENTIAL_ONLY            (1 << 2)
#define SMB_CREATEOPT_NO_INTERMEDIATE_BUFFERING  (1 << 3)
#define SMB_CREATEOPT_SYNCHRONOUS_IO_ALERT       (1 << 3)
#define SMB_CREATEOPT_SYNCHRONOUS_IO_NONALERTIF  (1 << 4)
#define SMB_CREATEOPT_NON_DIRECTORY_FILE         (1 << 5)
#define SMB_CREATEOPT_CREATE_TREE_CONNECTION     (1 << 6)
#define SMB_CREATEOPT_COMPLETE_IF_OPLOCKED       (1 << 7)
#define SMB_CREATEOPT_NO_EA_KNOWLEDGE            (1 << 8)
#define SMB_CREATEOPT_OPEN_FOR_RECOVERY          (1 << 9)
#define SMB_CREATEOPT_RANDOM_ACCESS              (1 << 10)
#define SMB_CREATEOPT_DELETE_ON_CLOSE            (1 << 11)
#define SMB_CREATEOPT_OPEN_BY_FILE_ID            (1 << 12)
#define SMB_CREATEOPT_OPEN_FOR_BACKUP_INTENT     (1 << 13)
#define SMB_CREATEOPT_NO_COMPRESSION             (1 << 14)
#define SMB_CREATEOPT_RESERVE_OPFILTER           (1 << 15)
#define SMB_CREATEOPT_OPEN_NO_RECALL             (1 << 16)
#define SMB_CREATEOPT_OPEN_FOR_FREE_SPACE_QUERY  (1 << 17)

// Security flags
#define SMB_SECURITY_NO_TRACKING            0
#define SMB_SECURITY_CONTEXT_TRACKING       (1 << 0)
#define SMB_SECURITY_EFFECTIVE_ONLY         (1 << 1)

// Write mode flags
#define SMB_WRITEMODE_WRITETHROUGH          (1 << 0)
#define SMB_WRITEMODE_READBYTESAVAILABLE    (1 << 1)
#define SMB_WRITEMODE_RAW                   (1 << 2)
#define SMB_WRITEMODE_MSG_START             (1 << 3)

// Trans 2 flags
//// Find First 2
#define SMB_FIND2_ATTR_RO       (1 << 0)  // Include RO files in result
#define SMB_FIND2_ATTR_HIDDEN   (1 << 1)  // Include hidden files
#define SMB_FIND2_ATTR_SYSTEM   (1 << 2)  // Include system files
#define SMB_FIND2_ATTR_VOLUME   (1 << 3)  // Include volume ID ?
#define SMB_FIND2_ATTR_DIR      (1 << 4)  // Include directory ?
#define SMB_FIND2_ATTR_ARCHIVE  (1 << 5)  // Include archive ?
#define SMB_FIND2_ATTR_DEFAULT  (SMB_FIND2_ATTR_RO | SMB_FIND2_ATTR_HIDDEN |  \
                                 SMB_FIND2_ATTR_SYSTEM | SMB_FIND2_ATTR_DIR)

#define SMB_FIND2_FLAG_CLOSE      (1 << 0)  // Close search after request ?
#define SMB_FIND2_FLAG_CLOSE_EOS  (1 << 1)  // Close after End Of Search ?
#define SMB_FIND2_FLAG_RESUME     (1 << 2)  // Send resume keys ?
#define SMB_FIND2_FLAG_CONTINUE   (1 << 3)  // not set == new search
#define SMB_FIND2_FLAG_BACKUP     (1 << 4)  // Backup intent ?

#endif
