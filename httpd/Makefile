## kc3
## Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = *.a kc3_httpd kc3_httpd_asan kc3_httpd_cov kc3_httpd_debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: kc3_httpd

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: kc3_httpd_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   kc3_httpd_cov

debug: kc3_httpd_debug

distclean:
	rm -rf ${DISTCLEANFILES}

fx: kc3_httpd
	httpd/.libs/kc3_httpd -C httpd/fx -d

fx_asan: kc3_httpd_asan
	httpd/.libs/kc3_httpd_asan -C httpd/fx -d

fx_cov: kc3_httpd_cov
	httpd/.libs/kc3_httpd_cov -C httpd/fx -d

fx_debug: kc3_httpd_debug
	httpd/.libs/kc3_httpd_debug -C httpd/fx -d

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3_httpd.html

gdb_fx: kc3_httpd_debug
	cd fx && gdb ../.libs/kc3_httpd_debug

gdb_fx_asan: kc3_httpd_asan
	cd fx && gdb ../.libs/kc3_httpd_asan

gdb_httpd: debug
	if [ -f kc3_httpd_debug.core ]; then gdb .libs/kc3_httpd_debug kc3_httpd_debug.core; else gdb .libs/kc3_httpd_debug; fi

install:
	mkdir -p ${prefix}/bin
	${INSTALL} -m 755 .libs/kc3_httpd ${prefix}/bin/kc3_httpd

lldb_httpd: debug
	if [ -f kc3_httpd_debug.core ]; then lldb .libs/kc3_httpd_debug kc3_httpd_debug.core; else lldb .libs/kc3_httpd_debug; fi

uninstall:
	rm -f ${prefix}/bin/kc3_httpd

.PHONY: \
	all \
	asan \
	clean \
	clean_cov \
	cov \
	debug \
	distclean \
	gdb_httpd \
	install \
	lldb_httpd \
	update_sources \
	uninstall

include config.mk
include sources.mk
