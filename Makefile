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



CFLAGS      	= -Iinclude -I contrib -DBDSM_DEBUG=1 -D_BSD_SOURCE -std=c99 -fPIC
LDFLAGS     	=
CC          	= clang
AR						= ar
RANLIB				= ranlib

LIB						= libdsm.so
LIB_STATIC		= libdsm.a
UTILS					= dsm discover inverse lookup

LIB_SRC				= contrib/mdx/md5.c \
			contrib/mdx/md4.c				\
			src/hmac_md5.c					\
			src/netbios_utils.c			\
			src/netbios_ns.c				\
			src/netbios_ns_entry.c	\
			src/netbios_query.c			\
			src/netbios_session.c		\
			src/smb_utils.c					\
			src/smb_message.c				\
			src/smb_session.c				\
			src/smb_ntlm.c					\
			src/smb_share.c					\
			src/smb_file.c					\
			src/smb_trans2.c				\
			src/context.c
DSM_SRC				= dsm.c
DISCOVER_SRC	= discover.c
INVERSE_SRC		= inverse.c
LOOKUP_SRC		= lookup.c

LIB_LDFLAGS		= -shared

LIB_OBJS			= $(LIB_SRC:.c=.o)

all: $(LIB) $(LIB_STATIC) $(UTILS)

doc:
	doxygen

clean:
	rm -f $(UTILS) $(LIB) $(LIB_STATIC) *.o
	rm -rf $(LIB_OBJS)

re: clean all
c: clean

$(LIB): $(LIB_OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(LIB_LDFLAGS) $(LIB_OBJS)
$(LIB_STATIC): $(LIB_OBJS)
	$(AR) -rcs $@ $(LIB_OBJS)

dsm: $(LIB) $(DSM_SRC)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(DSM_SRC) $(LIB_STATIC)
discover: $(LIB) $(DISCOVER_SRC)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(DISCOVER_SRC) $(LIB_STATIC)
inverse: $(LIB) $(INVERSE_SRC)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(INVERSE_SRC) $(LIB_STATIC)
lookup:  $(LIB) $(LOOKUP_SRC)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(LOOKUP_SRC) $(LIB_STATIC)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: all clean re doc
