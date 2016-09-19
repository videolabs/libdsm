ACLOCAL_AMFLAGS = -I m4

DISTCLEANFILES = @DOLT_CLEANFILES@

EXTRA_DIST = \
	abi_version.sh \
	package_version.sh \
	contrib/spnego/spnego.asn1 \
	contrib/spnego/spnego_asn1.c \
	src/libdsm.sym

CFLAGS = -I$(top_srcdir)/contrib -I$(top_srcdir)/include -I$(top_srcdir)/compat @TASN1_CFLAGS@ @CFLAGS@ @PTHREAD_CFLAGS@

if DEBUG
AM_CFLAGS = -O0 -g3
else
AM_CFLAGS = -O2 -DNDEBUG
endif

bdsmdir = $(includedir)/bdsm

bdsm_HEADERS = \
    include/bdsm.h      \
    include/bdsm/netbios_defs.h   \
    include/bdsm/netbios_ns.h   \
    include/bdsm/smb_defs.h   \
    include/bdsm/smb_dir.h   \
    include/bdsm/smb_file.h   \
    include/bdsm/smb_session.h    \
    include/bdsm/smb_share.h    \
    include/bdsm/smb_stat.h   \
    include/bdsm/smb_types.h
noinst_HEADERS = \
    compat/compat.h \
    compat/queue.h \
    src/bdsm_common.h    \
    src/bdsm_debug.h    \
    src/hmac_md5.h   \
    src/netbios_defs.h   \
    src/netbios_query.h  \
    src/netbios_session.h  \
    src/netbios_utils.h  \
    src/smb_buffer.h \
    src/smb_defs.h   \
    src/smb_dir.h    \
    src/smb_fd.h    \
    src/smb_file.h   \
    src/smb_message.h    \
    src/smb_packets.h     \
    src/smb_ntlm.h   \
    src/smb_session.h    \
    src/smb_share.h    \
    src/smb_stat.h   \
    src/smb_session_msg.h \
    src/smb_spnego.h      \
    src/smb_types.h    \
    src/smb_transport.h   \
    src/smb_utils.h    \
    contrib/spnego/spnego_asn1.h   \
    contrib/mdx/md4.h              \
    contrib/mdx/md5.h              \
    contrib/rc4/rc4.h

lib_LTLIBRARIES = libdsm.la

libdsm_la_SOURCES = \
    contrib/mdx/md4.c   \
    contrib/mdx/md5.c   \
    contrib/rc4/rc4.c   \
    contrib/spnego/spnego_asn1.c  \
    src/hmac_md5.c      \
    src/netbios_ns.c    \
    src/netbios_query.c     \
    src/netbios_session.c   \
    src/netbios_utils.c     \
    src/smb_buffer.c        \
    src/smb_dir.c           \
    src/smb_fd.c            \
    src/smb_file.c          \
    src/smb_spnego.c        \
    src/smb_message.c       \
    src/smb_ntlm.c          \
    src/smb_session.c       \
    src/smb_session_msg.c   \
    src/smb_share.c         \
    src/smb_stat.c          \
    src/smb_trans2.c        \
    src/smb_transport.c     \
    src/smb_utils.c

noinst_LTLIBRARIES = libcompat.la
libcompat_la_SOURCES = compat/compat.c
libcompat_la_LIBADD = $(LTLIBOBJS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdsm.pc

libdsm_la_LIBADD = libcompat.la $(TASN1_LIBS) @LTLIBICONV@
libdsm_la_LDFLAGS = -version-info @BDSM_LIBTOOL_VERSION@ \
	-no-undefined -export-symbols $(srcdir)/src/libdsm.sym

bin_PROGRAMS =

if PROGRAMS
bin_PROGRAMS += dsm dsm_discover dsm_inverse dsm_lookup
endif

dsm_SOURCES = bin/dsm.c

dsm_discover_SOURCES = bin/discover.c

dsm_inverse_SOURCES = bin/inverse.c

dsm_lookup_SOURCES = bin/lookup.c

LDADD = libdsm.la

clean-local:
	rm -rf $(top_builddir)/doc/html
	rm -rf $(top_builddir)/contrib/spnego/spnego_asn1.c

if HAVE_DOXYGEN
doc:
	$(DOXYGEN) doc/Doxyfile
else
doc:
	echo "Doxygen wasn't detected by configure, it's support has been disabled"
endif

contrib/spnego/spnego_asn1.c: contrib/spnego/spnego.asn1
	asn1Parser -o $@ -n spnego_asn1_conf $<

a : all
c : clean
re : clean all

.PHONY: c a re doc
