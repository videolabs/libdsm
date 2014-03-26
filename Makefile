# Makefile for libdsm

CFLAGS      = -g3 -I include -DBDSM_DEBUG=1 -D_BSD_SOURCE -std=c99
LDFLAGS     = -levent
CC          = clang

UTIL				= dsm
LIB					= libdsm.so

UTIL_SRC		= dsm.c 							    \
							src/netbios_utils.c	    \
							src/netbios_ns.c		    \
							src/netbios_ns_entry.c	\
							src/netbios_query.c     \
							src/netbios_session.c		\
							src/context.c

UTIL_OBJS		= $(UTIL_SRC:.c=.o)

all: $(UTIL)

clean:
	rm -f $(UTIL) $(UTIL_OBJS)

re: clean all

$(UTIL): $(UTIL_OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(UTIL_OBJS)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: all clean re
