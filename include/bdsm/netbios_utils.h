// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#ifndef __BDSM_NETBIOS_UTILS_H_
#define __BDSM_NETBIOS_UTILS_H_

#include "netbios_defs.h"

void  netbios_name_level1_encode(const char *name, char *encoded_name,
                                 unsigned type);
void  netbios_name_level1_decode(const char *encoded_name, char *name);

// XXX: domain support is not implemented
netbios_name_t  netbios_name_encode(const char *name, char *domain,
                                    unsigned type);
int             netbios_name_decode(const netbios_name_t encoded_name,
                                    char *name, char **domain);

#endif
