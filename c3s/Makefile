## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software excepted
## on Apple computers granted the above copyright notice and
## this permission paragraph are included in all copies and
## substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = *.a c3s c3s_asan c3s_cov c3s_debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: c3s

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: c3s_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   c3s_cov

debug: c3s_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details c3s.html

gdb_c3s: debug
	if [ -f c3s_debug.core ]; then gdb c3s_debug c3s_debug.core; else gdb c3s_debug; fi

install:
	mkdir -p ${prefix}/bin
	install -m 755 c3s ${prefix}/bin/c3s

.PHONY: all asan cov debug clean clean_cov distclean

include config.mk
include sources.mk
