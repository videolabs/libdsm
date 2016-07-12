AC_PREREQ([2.53])

AC_COPYRIGHT([Copyright 2014-2016 VideoLabs])

AC_INIT([libdsm], [m4_esyscmd([./package_version.sh])], [], [libdsm], [])
AC_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([include/bdsm.h])
AC_CONFIG_LIBOBJ_DIR([compat])
AM_INIT_AUTOMAKE([1.6 foreign subdir-objects tar-ustar dist-zip])
m4_ifdef([AM_SILENT_RULES], [
  AM_SILENT_RULES([yes])
])

AC_CANONICAL_HOST

BDSM_ABI_VERSION=m4_esyscmd([./abi_version.sh])
BDSM_PACKAGE_VERSION=m4_esyscmd([./package_version.sh])
BDSM_LIBTOOL_VERSION=m4_esyscmd([./abi_version.sh -libtool])

AC_SUBST(BDSM_ABI_VERSION)
AC_SUBST(BDSM_PACKAGE_VERSION)
AC_SUBST(BDSM_LIBTOOL_VERSION)

AC_MSG_NOTICE([dsm package version: $BDSM_PACKAGE_VERSION])
AC_MSG_NOTICE([dsm ABI version: $BDSM_ABI_VERSION])

AC_PROG_CC_C99
AM_PROG_CC_C_O

AC_DEFINE([_FORTIFY_SOURCE], 2, [Define to 2 to get glibc warnings.])
AC_DEFINE([_FILE_OFFSET_BITS], 64, [Define to 64 for large files support.])

dnl check for -Wall -Wextra support
CC_CHECK_CFLAG_APPEND([-Wall -Wsign-compare -Wextra -Wstrict-aliasing -Wstrict-overflow])

dnl Where is iconv? In libc or separate lib?
AM_ICONV

AC_ARG_ENABLE([programs],
  AS_HELP_STRING([--enable-programs], [Build additional programs [default=yes]])
)

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Additional debugging features [default=no]])
)

AM_CONDITIONAL([DEBUG], [test x"$enable_debug" == x"yes"])
AM_CONDITIONAL([PROGRAMS], [test x"$enable_programs" != x"no"])

LT_INIT

DOLT

AC_CONFIG_MACRO_DIR([m4])

dnl Check for pthreads
AX_PTHREAD(,[
    AC_CHECK_LIB(pthreadGC2,pthread_join,[
        AC_SUBST(PTHREAD_LIBS,"-lpthreadGC2")
        AC_SUBST(PTHREAD_CFLAGS,"-DPTW32_STATIC_LIB"])
    ])
])

##############################
## Checks for the ASN.1 parser
PKG_CHECK_MODULES([TASN1], [libtasn1])

## Check if it supports new types
AC_MSG_CHECKING([whether libtasn1 version >= 3.0])
CFLAGS="$CFLAGS $TASN1_CFLAGS"
AC_COMPILE_IFELSE([
  AC_LANG_PROGRAM([#include <libtasn1.h>], [ asn1_node test; ])
], [
  AC_MSG_RESULT([yes])
  HAVE_TASN1_3PLUS=yes
], [
  AC_MSG_RESULT([no])
  HAVE_TASN1_3PLUS=no
])

AM_CONDITIONAL([HAVE_TASN1_3PLUS],[test x$HAVE_TASN1_3PLUS = xyes])
AS_IF([test x"$HAVE_TASN1_3PLUS" = x"yes"], [
  AC_DEFINE([HAVE_TASN1_3PLUS], [1], [libtasn1 has v 3.0 downcase types])

  dnl Ugly compatibility with old 2.x types
  AC_DEFINE([ASN1_ARRAY_TYPE], [asn1_static_node], [...])
], [
  dnl Ugly compatibility with new 3.x types
  AC_DEFINE([asn1_static_node], [ASN1_ARRAY_TYPE], [...])
])

AC_SEARCH_LIBS([strlcpy], [bsd], [
  AC_DEFINE([HAVE_LIBBSD], [1], [Does this system have libbsd strl*** functions implementation])
])
AC_SEARCH_LIBS([clock_gettime], [rt pthread])
AC_CHECK_TYPES([clockid_t],,,[
               #include <time.h>
               #include <pthread.h>])
AC_CHECK_TYPES([struct timespec],,,[#include <time.h>])
AC_CHECK_DECLS([CLOCK_MONOTONIC],,,[
               #include <time.h>
               #ifdef _WIN32
               #include <pthread_time.h>
               #endif])
AC_REPLACE_FUNCS([strlcpy])
AC_REPLACE_FUNCS([strndup])
AC_REPLACE_FUNCS([clock_gettime])
AC_CHECK_FUNCS([pipe _pipe getifaddrs])

AC_CHECK_HEADERS([bsd/string.h langinfo.h alloca.h sys/queue.h arpa/inet.h sys/socket.h ifaddrs.h])

## Configure random device path
AC_ARG_WITH([urandom], 
    [AS_HELP_STRING([--with-urandom=PATH], 
        [Configure the path of the random generation device used @<:@default=/dev/urandom@:>@ ])],
    [], [with_urandom=/dev/urandom])
AC_DEFINE_UNQUOTED([URANDOM], ["$with_urandom"], [Path of the random number generation device])

## Check for doxygen presence
AC_CHECK_PROGS([DOXYGEN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
if test -z "$DOXYGEN";
  then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
else
  AC_CONFIG_FILES([doc/Doxyfile])
fi

case "${host_os}" in
  *android*)
    BDSM_LIB_LOG="-llog"
    LDFLAGS="${LDFLAGS} ${BDSM_LIB_LOG}"
    ;;
  *mingw32* | *cygwin* | *wince* | *mingwce*)
    AC_DEFINE([_UNICODE], [1], [Define to 1 for Unicode (Wide Chars) APIs.])
    AC_DEFINE([UNICODE], [1], [Define to 1 for Unicode (Wide Chars) APIs.])
    AC_DEFINE([_ISOC99_SOURCE], [1], [Extensions to ISO C89 from ISO C99.])
    AC_DEFINE([_POSIX_SOURCE], [1], [IEEE Std 1003.1.])
    AC_DEFINE([_POSIX_C_SOURCE], [200809L], [IEEE Std 1003.1.])
    SOCKET_LIBS="-lws2_32"
    LDFLAGS="${LDFLAGS} ${SOCKET_LIBS}"
    ;;
esac
AC_SUBST(BDSM_LIB_LOG)
AC_SUBST(SOCKET_LIBS)

AC_OUTPUT([
Makefile
libdsm.pc
])
