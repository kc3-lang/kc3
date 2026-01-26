## kc3
## Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

build: ${PROG}

asan: ${PROG_ASAN}

cov: ${PROG_COV}

debug: ${PROG_DEBUG}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3s.html

gdb_kc3s: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

install:
	mkdir -p ${DESTDIR}${prefix}/bin
	install -m 755 ${PROG} ${DESTDIR}${prefix}/bin/

uninstall:
	rm -f ${prefix}/bin/kc3s

.PHONY: all asan cov debug install uninstall
