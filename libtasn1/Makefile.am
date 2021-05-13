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

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

EXTRA_DIST = windows/asn1-parser/asn1-parser.vcproj			\
	windows/asn1-parser/config.h windows/libtasn1.ncb		\
	windows/libtasn1.sln windows/libtasn1.suo			\
	windows/gnulib/getopt.h windows/gnulib/gnulib.vcproj		\
	windows/libtasn1/libtasn1.vcproj				\
	windows/test-parser/test-parser.vcproj				\
	windows/test-tree/test-tree.vcproj windows/libtasn14win.mk

SUBDIRS = gl lib src examples tests

if ENABLE_DOC
SUBDIRS += doc
endif

ACLOCAL_AMFLAGS = -I m4 -I gl/m4 -I lib/glm4

EXTRA_DIST += cfg.mk maint.mk .clcopying

gnulib-update:
	gnulib-tool --add-import --dir=. --lib=libgnu --source-base=lib/gllib --m4-base=lib/glm4 --doc-base=doc --tests-base=tests --aux-dir=build-aux --lgpl=2 --no-conditional-dependencies --libtool --macro-prefix=lgl --no-vc-files

TMPFILE="abi-temp.xml"

abi-check:
	@rm -f $(TMPFILE)
	@echo "Checking libtasn1 ABI"
	@echo "<version>$(VERSION)</version>" >$(TMPFILE)
	@echo "<headers>$(srcdir)/lib/libtasn1.h" >>$(TMPFILE)
	@echo "$(builddir)/lib/libtasn1.h</headers>" >>$(TMPFILE)
	@echo "<libs>$(builddir)/lib/.libs</libs>" >>$(TMPFILE)
	test ! -f "$(srcdir)/devel/ABI-$$(uname -m).dump" || \
		abi-compliance-checker -abi -lib libtasn1 -old "$(srcdir)/devel/ABI-$$(uname -m).dump" -new $(TMPFILE)
	@rm -f $(TMPFILE)

dist-hook: abi-check
	make -C doc/ compare-makefile
