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

#ifndef _NETBIOS_UTILS_H_
#define _NETBIOS_UTILS_H_

#include "netbios_defs.h"

void  netbios_name_level1_encode(const char *name, char *encoded_name,
                                 unsigned type);
void  netbios_name_level1_decode(const char *encoded_name, char *name);

// XXX: domain support is not implemented
char  *netbios_name_encode(const char *name, char *domain,
                           unsigned type);
int   netbios_name_decode(const char *encoded_name,
                          char *name, char **domain);

#endif
