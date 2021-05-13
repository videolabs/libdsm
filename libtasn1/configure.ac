dnl Process this file with autoconf to produce a configure script.
# Copyright (C) 2002-2014 Free Software Foundation, Inc.
#
# This file is part of LIBTASN1.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.61])
AC_INIT([GNU Libtasn1],[4.9],[help-libtasn1@gnu.org])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.10 -Wall -Wno-override])
AM_SILENT_RULES([yes])

# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 11)
AC_SUBST(LT_REVISION, 2)
AC_SUBST(LT_AGE, 5)

AC_PROG_CC
gl_EARLY
lgl_EARLY
AC_PROG_YACC

AC_ARG_ENABLE(doc,
  AS_HELP_STRING([--disable-doc], [don't generate any documentation]),
    enable_doc=$enableval, enable_doc=yes)
AM_CONDITIONAL(ENABLE_DOC, test "$enable_doc" != "no")

dnl Checks for programs.
AM_MISSING_PROG(HELP2MAN, help2man, $missing_dir)
AC_LIBTOOL_WIN32_DLL
AM_PROG_AR
AM_PROG_LIBTOOL
GTK_DOC_CHECK(1.2)

AC_CHECK_SIZEOF(unsigned long int, 4)
AC_CHECK_SIZEOF(unsigned int, 4)

sj_UPDATE_HEADER_VERSION([$srcdir/lib/libtasn1.h])

# For gnulib compatibility modules.
gl_INIT
lgl_INIT

AC_ARG_ENABLE([gcc-warnings],
  [AS_HELP_STRING([--disable-gcc-warnings],
		  [disable GCC warnings (for developers)])],
  [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for gcc-warnings option]) ;;
   esac
   gl_gcc_warnings=$enableval],
  [gl_gcc_warnings=yes]
)

if test "$gl_gcc_warnings" = yes; then
  gl_WARN_ADD([-Werror], [WERROR_CFLAGS])

  nw="$nw -Wsystem-headers"         # Don't let system headers trigger warnings
  nw="$nw -Wc++-compat"             # We don't care strongly about C++ compilers
  nw="$nw -Wtraditional"            # Warns on #elif which we use often
  nw="$nw -Wtraditional-conversion" # Too many warnings for now
  nw="$nw -Wconversion"             # Too many warnings for now
  nw="$nw -Wsign-conversion"        # Too many warnings for now
  nw="$nw -Wold-style-definition"   #
  nw="$nw -Wpadded"                 # Our structs are not padded
  nw="$nw -Wundef"                  #
  nw="$nw -Wunreachable-code"       # Too many false positives
  nw="$nw -Wunused-macros"          # Breaks on bison generated ASN1.c
  nw="$nw -Wswitch-default"         # Breaks on bison generated ASN1.c
  nw="$nw -Wunsafe-loop-optimizations"
  nw="$nw -Wstrict-overflow"
  nw="$nw -Wsuggest-attribute=pure" # Is it worth using pure attributes?

  gl_MANYWARN_ALL_GCC([ws])
  gl_MANYWARN_COMPLEMENT(ws, [$ws], [$nw])
  for w in $ws; do
    gl_WARN_ADD([$w])
  done

  gl_WARN_ADD([-Wno-type-limits])
  gl_WARN_ADD([-Wno-missing-field-initializers])
  gl_WARN_ADD([-Wno-unused-parameter])
  gl_WARN_ADD([-Wno-stack-protector])            # Some functions cannot be protected
  gl_WARN_ADD([-fdiagnostics-show-option])
fi

AC_CONFIG_FILES([
  Makefile
  doc/Makefile
  doc/cyclo/Makefile
  doc/reference/Makefile
  doc/reference/version.xml
  examples/Makefile
  gl/Makefile
  lib/Makefile
  lib/gllib/Makefile
  lib/libtasn1.pc
  src/Makefile
  tests/Makefile
])
AC_OUTPUT

AC_MSG_NOTICE([summary of build options:

  version:          ${VERSION} shared $LT_CURRENT:$LT_REVISION:$LT_AGE
  Host type:        ${host}
  Install prefix:   ${prefix}
  Compiler:         ${CC}
  Warning flags:    errors: ${WERROR_CFLAGS} warnings: ${WARN_CFLAGS}
  Library types:    Shared=${enable_shared}, Static=${enable_static}
  Valgrind:         ${VALGRIND}
  Version script:   $have_ld_version_script
])
