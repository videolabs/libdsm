AC_PREREQ([2.53])

AC_INIT([libdsm], [m4_esyscmd([./package_version.sh])],
  [], [libdsm], [])
AC_CONFIG_SRCDIR([include/bdsm.h])
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


AC_ARG_ENABLE([programs],
  AS_HELP_STRING([--enable-programs], [Build additional programs [default=yes]])
)

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Additional debugging features [default=yes]])
)

AS_IF([test x"$enable_debug" != x"no"], [
  AC_DEFINE([BDSM_DEBUG], [1])
], [
  AC_DEFINE([BDSM_DEBUG], [0])
])

AM_CONDITIONAL([PROGRAMS], [test x"$enable_programs" != x"no"])

LT_INIT

DOLT

AC_CONFIG_MACRO_DIR([m4])

## Checks for doxygen presence
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
  then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AC_OUTPUT([
Makefile
libdsm.pc
])
