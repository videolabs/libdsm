# Copyright (C) 2006-2014 Free Software Foundation, Inc.
# Author: Simon Josefsson
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

WFLAGS ?= --enable-gcc-warnings
ADDFLAGS ?=
CFGFLAGS ?= --enable-gtk-doc --enable-gtk-doc-pdf $(ADDFLAGS) $(WFLAGS)

INDENT_SOURCES = `find . -name \*.[ch]|grep -v -e ^./gl -e ^./lib/gl -e ^./build-aux/ -e ^./lib/ASN1.c -e ^./tests/Test_tree_asn1_tab.c`

ifeq ($(.DEFAULT_GOAL),abort-due-to-no-makefile)
.DEFAULT_GOAL := bootstrap
endif

local-checks-to-skip = sc_prohibit_strcmp sc_prohibit_have_config_h	\
	sc_require_config_h sc_require_config_h_first			\
	sc_immutable_NEWS sc_prohibit_magic_number_exit			\
	sc_bindtextdomain
VC_LIST_ALWAYS_EXCLUDE_REGEX = ^(maint.mk|gtk-doc.make|build-aux/.*|gl/.*|lib/gllib/.*|lib/glm4/.*|lib/ASN1\.c|m4/pkg.m4|doc/gdoc|windows/.*|doc/fdl-1.3.texi)$$
update-copyright-env = UPDATE_COPYRIGHT_USE_INTERVALS=1

# Explicit syntax-check exceptions.
exclude_file_name_regexp--sc_prohibit_empty_lines_at_EOF = ^tests/TestIndef.p12$$
exclude_file_name_regexp--sc_GPL_version = ^lib/libtasn1.h$$
exclude_file_name_regexp--sc_program_name = ^tests/|examples/
exclude_file_name_regexp--sc_prohibit_atoi_atof = ^src/asn1Coding.c|src/asn1Decoding.c$$
exclude_file_name_regexp--sc_prohibit_empty_lines_at_EOF = ^tests/crlf.cer|tests/TestIndef.p12$$
exclude_file_name_regexp--sc_error_message_uppercase = ^tests/Test_tree.c$$
exclude_file_name_regexp--sc_unmarked_diagnostics = ^tests/Test_tree.c$$

bootstrap-tools := autoconf,automake,libtool,bison
gpg_key_ID = b565716f

autoreconf:
	touch ChangeLog
	test -f ./configure || autoreconf --install

bootstrap: autoreconf
	./configure $(CFGFLAGS)

review-diff:
	git diff `git describe --abbrev=0`.. \
	| grep -v -e ^index -e '^diff --git' \
	| filterdiff -p 1 -x 'gl/*' -x 'build-aux/*' -x 'lib/gl*' -x 'po/*' -x 'maint.mk' -x '.gitignore' -x '.x-sc*' -x ChangeLog -x GNUmakefile -x 'lib/ASN1.c' \
	| less

# Release

htmldir = ../www-$(PACKAGE)

coverage-copy:
	rm -fv `find $(htmldir)/coverage -type f | grep -v CVS`
	mkdir -p $(htmldir)/coverage/
	cp -rv $(COVERAGE_OUT)/* $(htmldir)/coverage/

coverage-upload:
	cd $(htmldir) && \
	find coverage -type d -! -name CVS -! -name '.' \
		-exec cvs add {} \; && \
	find coverage -type d -! -name CVS -! -name '.' \
		-exec sh -c "cvs add -kb {}/*.png" \; && \
	find coverage -type d -! -name CVS -! -name '.' \
		-exec sh -c "cvs add {}/*.html" \; && \
	cvs add coverage/libtasn1.info coverage/gcov.css || true && \
	cvs commit -m "Update." coverage

clang:
	make clean
	scan-build ./configure
	rm -rf scan.tmp
	scan-build -o scan.tmp make

clang-copy:
	rm -fv `find $(htmldir)/clang-analyzer -type f | grep -v CVS`
	mkdir -p $(htmldir)/clang-analyzer/
	cp -rv scan.tmp/*/* $(htmldir)/clang-analyzer/

clang-upload:
	cd $(htmldir) && \
		cvs add clang-analyzer || true && \
		cvs add clang-analyzer/*.css clang-analyzer/*.js \
			clang-analyzer/*.html || true && \
		cvs commit -m "Update." clang-analyzer

cyclo-copy:
	cp -v doc/cyclo/cyclo-$(PACKAGE).html $(htmldir)/cyclo/index.html

cyclo-upload:
	cd $(htmldir) && cvs commit -m "Update." cyclo/index.html

gendoc-copy:
	cd doc && $(SHELL) ../build-aux/gendocs.sh \
		--html "--css-include=texinfo.css" \
		-o ../$(htmldir)/manual/ $(PACKAGE) "$(PACKAGE_NAME)"

gendoc-upload:
	cd $(htmldir) && \
		cvs add manual || true && \
		cvs add manual/html_node || true && \
		cvs add -kb manual/*.gz manual/*.pdf || true && \
		cvs add manual/*.txt manual/*.html \
			manual/html_node/*.html || true && \
		cvs commit -m "Update." manual/

gtkdoc-copy:
	mkdir -p $(htmldir)/reference/
	cp -v doc/reference/$(PACKAGE).pdf \
		doc/reference/html/*.html \
		doc/reference/html/*.png \
		doc/reference/html/*.devhelp2 \
		doc/reference/html/*.css \
		$(htmldir)/reference/

gtkdoc-upload:
	cd $(htmldir) && \
		cvs add reference || true && \
		cvs add -kb reference/*.png reference/*.pdf || true && \
		cvs add reference/*.html reference/*.css \
			reference/*.devhelp2 || true && \
		cvs commit -m "Update." reference/

ChangeLog:
	git2cl > ChangeLog
	cat .clcopying >> ChangeLog

tag = $(PACKAGE)_`echo $(VERSION) | sed 's/\./_/g'`

tarball:
	! git tag -l $(tag) | grep $(PACKAGE) > /dev/null
	rm -f ChangeLog
	$(MAKE) ChangeLog distcheck

libtasn14win:
	cd windows && make -f libtasn14win.mk libtasn14win VERSION=$(VERSION)

libtasn14win-upload:
	cd windows && make -f libtasn14win.mk upload VERSION=$(VERSION)

source:
	git tag -u b565716f -m $(VERSION) $(tag)

release-check: syntax-check tarball libtasn14win gendoc-copy gtkdoc-copy coverage coverage-copy clang clang-copy

release-upload-www: gendoc-upload gtkdoc-upload coverage-upload clang-upload

release-upload-ftp:
	build-aux/gnupload --to ftp.gnu.org:$(PACKAGE) $(distdir).tar.gz
	cp $(distdir).tar.gz $(distdir).tar.gz.sig ../releases/$(PACKAGE)/
	git push
	git push --tags

release: release-check release-upload-www source release-upload-ftp libtasn14win-upload
