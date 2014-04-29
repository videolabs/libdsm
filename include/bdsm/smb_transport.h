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

#ifndef __BDSM_SMB_TRANSPORT_H_
#define __BDSM_SMB_TRANSPORT_H_

#include "bdsm/smb_types.h"

/**
 * @internal
 * @brief Fill the smb_transport structure with the fun pointers for using
 * NBT transport
 */
void              smb_transport_nbt(smb_transport *tr);
/**
 * @internal
 * @brief Fill the smb_transport structure with the fun pointers for using
 * DirectTCP transport
 */
void              smb_transport_tcp(smb_transport *tr);

#endif
