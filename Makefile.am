ACLOCAL_AMFLAGS = -I m4

DISTCLEANFILES = @DOLT_CLEANFILES@

EXTRA_DIST =

CFLAGS = -I$(top_srcdir)/contrib -I$(top_srcdir)/include

if DEBUG
AM_CFLAGS = -O0 -g3 -Wall #-Wextra
else
AM_CFLAGS = -O2
endif

bdsmdir = $(includedir)/bdsm

bdsm_HEADERS = \
    include/bdsm.h      \
    include/bdsm/debug.h    \
    include/bdsm/hmac_md5.h   \
    include/bdsm/netbios_defs.h   \
    include/bdsm/netbios_ns.h   \
    include/bdsm/netbios_query.h  \
    include/bdsm/netbios_session.h  \
    include/bdsm/netbios_utils.h  \
    include/bdsm/smb_defs.h   \
    include/bdsm/smb_fd.h    \
    include/bdsm/smb_file.h   \
    include/bdsm/smb_message.h    \
    include/bdsm/smb_ntlm.h   \
    include/bdsm/smb_packets.h    \
    include/bdsm/smb_session.h    \
    include/bdsm/smb_share.h    \
    include/bdsm/smb_stat.h   \
    include/bdsm/smb_types.h    \
    include/bdsm/smb_utils.h

lib_LTLIBRARIES = libdsm.la

libdsm_la_SOURCES = \
    contrib/mdx/md4.c   \
    contrib/mdx/md5.c   \
    src/hmac_md5.c    \
    src/netbios_ns.c    \
    src/netbios_ns_entry.c  \
    src/netbios_query.c   \
    src/netbios_session.c \
    src/netbios_utils.c   \
    src/smb_fd.c   \
    src/smb_file.c    \
    src/smb_message.c   \
    src/smb_ntlm.c    \
    src/smb_session.c   \
    src/smb_share.c   \
    src/smb_stat.c    \
    src/smb_trans2.c  \
    src/smb_utils.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdsm.pc

libdsm_la_LDFLAGS = -version-info @BDSM_LIBTOOL_VERSION@ LTLIBICONV


bin_PROGRAMS =

if PROGRAMS
bin_PROGRAMS += dsm dsm_discover dsm_inverse dsm_lookup
endif

dsm_SOURCES = bin/dsm.c

dsm_discover_SOURCES = bin/discover.c

dsm_inverse_SOURCES = bin/inverse.c

dsm_lookup_SOURCES = bin/lookup.c

LDADD = libdsm.la

if HAVE_DOXYGEN
doc:
	$(DOXYGEN) doc/Doxyfile
clean-local:
	rm -rf $(top_builddir)/doc/html
else
doc:
	echo "Doxygen wasn't detected by configure, it's support has been disabled"
endif

a : all
c : clean
re : clean all

.PHONY: c a re doc
