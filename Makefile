##---------------------------------------------------------------------------
##  __________________    _________  _____            _____  .__         ._.
##  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
##   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
##   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
##   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
##          \/        \/        \/        \/  )/           \/        \/   \/
##
## This file is part of libdsm. Copyright © 2014 VideoLabs SAS
##
## Author: Julien 'Lta' BALLET <contact@lta.io>
##
## This program is free software. It comes without any warranty, to the extent
## permitted by applicable law. You can redistribute it and/or modify it under
## the terms of the Do What The Fuck You Want To Public License, Version 2, as
## published by Sam Hocevar. See the COPYING file for more details.
##----------------------------------------------------------------------------



CFLAGS      = -g3 -Iinclude -DBDSM_DEBUG=1 -D_BSD_SOURCE -std=c99 #-Wall -Wextra
LDFLAGS     = -lcrypto #-levent
CC          = clang

UTIL				= dsm
LIB					= libdsm.so

UTIL_SRC		= dsm.c 							    \
			src/netbios_utils.c				\
			src/netbios_ns.c				\
			src/netbios_ns_entry.c				\
			src/netbios_query.c				\
			src/netbios_session.c				\
			src/smb_utils.c					\
			src/smb_message.c				\
			src/smb_session.c				\
			src/smb_ntlm.c					\
			src/smb_share.c					\
			src/smb_file.c					\
			src/context.c

UTIL_OBJS		= $(UTIL_SRC:.c=.o)

all: $(UTIL)

clean:
	rm -f $(UTIL) $(UTIL_OBJS)

re: clean all
c: clean

$(UTIL): $(UTIL_OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(UTIL_OBJS)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: all clean re
