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

#ifndef __BSDM_SMB_DEFS_H_
#define __BSDM_SMB_DEFS_H_

#include <bdsm/smb_packets.h>

#define SMB_MAGIC               { 0xff, 0x53, 0x4d, 0x42 } // aka "\xffSMB"

#define SMB_DIALECTS {          \
  "\2Samba",                    \
  "\2NT LM 0.12",               \
  NULL                          \
}

// Dialect values must match position on SMB_DIALECTS array
#define SMB_DIALECT_SAMBA       0
#define SMB_DIALECT_NTLM        1

#define SMB_CMD_CLOSE           0x04
#define SMD_CMD_TRANS           0x25
#define SMB_CMD_TRANS2          0x32
#define SMB_CMD_TREE_DISCONNECT 0x71
#define SMB_CMD_NEGOTIATE       0x72
#define SMB_CMD_SETUP           0x73 // Session Setup AndX
#define SMB_CMD_TREE_CONNECT    0x75 // Tree Connect AndX
#define SMB_CMD_ECHO            0x2b
#define SMB_CMD_READ            0x2e // Read AndX
#define SMB_CMD_CREATE          0xa2 // NT Create AndX

///////////////////////////////////////////////////////////////////////////////
//// Flags definitions

// Many aren't use in libdsm but are here for possible later use

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
#define SMB_FLAG_EXT_SEC        (1 << (11 + 8))
#define SMB_FLAG_REPARSE_PATH   (1 << (10 + 8))
#define SMB_FLAG_LONG_NAMES     (1 << (6 + 8))
#define SMB_FLAG_SIGN_REQUIRED  (1 << (4 + 8))
#define SMB_FLAG_COMPRESSED     (1 << (3 + 8))
#define SMB_FLAG_SIGN_SUPPORT   (1 << (2 + 8))
#define SMB_FLAG_EXT_ATTR       (1 << (1 + 8))
#define SMB_FLAG_LONG_NAMES_OK  (1 << (0 + 8))
// File creation/open flags
#define SMB_CREATE_OPLOCK       (1 << 1)
#define SMB_CREATE_BATCH_OPLOCK (1 << 2)
#define SMB_CREATE_MKDIR        (1 << 3)
#define SMB_CREATE_EXT_RESP     (1 << 4)
#define SMB_CREATE_DEFAULTS     (0)
// File access rights
#define SMB_MOD_READ            (1 << 0)
#define SMB_MOD_WRITE           (1 << 1)
#define SMB_MOD_APPEND          (1 << 2)
#define SMB_MOD_READ_EXT        (1 << 3)
#define SMB_MOD_WRITE_EXT       (1 << 4)
#define SMB_MOD_EXEC            (1 << 5)
#define SMB_MOD_RMCHILD         (1 << 6)
#define SMB_MOD_READ_ATTR       (1 << 7)
#define SMB_MOD_WRITE_ATTR      (1 << 8)
#define SMB_MOD_RM              (1 << 16)
#define SMB_MOD_READ_CTL        (1 << 17)
#define SMB_MOD_WRITE_DAC       (1 << 18)
#define SMB_MOD_CHOWN           (1 << 19)
#define SMB_MOD_SYNC            (1 << 20)
#define SMB_MOD_SYS             (1 << 24)
#define SMB_MOD_MAX_ALLOWED     (1 << 25)
#define SMB_MOD_GENERIC_ALL     (1 << 28)
#define SMB_MOD_GENERIC_EXEC    (1 << 29)
#define SMB_MOD_GENERIC_READ    (1 << 30)
#define SMB_MOD_GENERIC_WRITE   (1 << 31)
#define SMB_MOD_RW              (SMB_MOD_READ | SMB_MOD_WRITE | SMB_MOD_APPEND \
                                | SMB_MOD_READ_EXT | SMB_MOD_WRITE_EXT \
                                | SMB_MOD_READ_ATTR | SMB_MOD_WRITE_ATTR \
                                | SMB_MOD_READ_CTL )
#define SMB_MOD_RO              (SMB_MOD_READ | SMB_MOD_READ_EXT \
                                | SMB_MOD_READ_ATTR | SMB_MOD_READ_CTL )

// File attributes
#define SMB_ATTR_RO             (1 << 0)
#define SMB_ATTR_HIDDEN         (1 << 1)
#define SMB_ATTR_SYS            (1 << 2)
#define SMB_ATTR_VOLID          (1 << 3)  // Volume ID
#define SMB_ATTR_DIR            (1 << 4)
#define SMB_ATTR_ARCHIVE        (1 << 5)  // Modified since last archive (!?)
#define SMB_ATTR_DEVICE         (1 << 6)
#define SMB_ATTR_NORMAL         (1 << 7)
#define SMB_ATTR_TEMP           (1 << 8)
#define SMB_ATTR_SPARSE         (1 << 9)
#define SMB_ATTR_REPARSE_PT     (1 << 10)
#define SMB_ATTR_COMPRESSED     (1 << 11)
#define SMB_ATTR_OFFLINE        (1 << 12)
#define SMB_ATTR_INDEXED        (1 << 13) // Not set = May be indexed
#define SMB_ATTR_ENCRYPTED      (1 << 14)
// Share access flags
#define SMB_SHARE_READ          (1 << 0)
#define SMB_SHARE_WRITE         (1 << 1)
#define SMB_SHARE_DELETE        (1 << 2)





#define SMB_NTLM_HASH_SIZE      16
#define SMB_NTLM2_BLOB_SIZE     64
#define SMB_LM2_BLOB_SIZE       8

#define SMB_OS                  "Unix"
#define SMB_LANMAN              "libdsm-dev"

#define NT_STATUS_SUCCESS                   0x00000000
#define NT_STATUS_MORE_PROCESSING_REQUIRED  0xc0000016
#define NT_STATUS_ACCESS_DENIED             0xc0000022


#endif
