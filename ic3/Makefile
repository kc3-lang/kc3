## c3
## Copyright 2022 kmx.io <contact@kmx.io>
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

CLEANFILES = *.a ic3 ic3.asan ic3.cov ic3.debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: ic3

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: ic3.asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   ic3.cov

debug: ic3.debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ic3.html

gdb_ic3: debug
	if [ -f ic3.debug.core ]; then gdb ic3.debug ic3.debug.core; else gdb ic3.debug; fi

.PHONY: all asan cov debug clean clean_cov distclean

include config.mk
include sources.mk
