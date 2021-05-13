## Process this file with automake to produce Makefile.in
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

AM_CFLAGS = $(WARN_CFLAGS) $(WERROR_CFLAGS)
AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/gl -I$(top_builddir)/gl

LDADD = ../lib/libtasn1.la ../gl/libgnu.la

bin_PROGRAMS = asn1Parser asn1Coding asn1Decoding

asn1Parser_SOURCES = asn1Parser.c

asn1Coding_SOURCES = asn1Coding.c

asn1Decoding_SOURCES = asn1Decoding.c benchmark.c benchmark.h
