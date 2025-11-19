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
	gcovr --gcov-executable ${GCOV} --html-details ikc3.html

gdb_ikc3: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

gdb_ikc3_asan: asan
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_ASAN} ${PROG_ASAN}.core; else gdb ${PROG_ASAN}; fi

install:
	install -m 755 -d ${DESTDIR}${prefix}/bin
	${INSTALL} -m 755 ${PROG} ${DESTDIR}${prefix}/bin/${PROG}
	if ${HAVE_ASAN}; then \
	    ${INSTALL} -m 755 ${PROG_ASAN} ${DESTDIR}${prefix}/bin/; fi
	if ${HAVE_GCOV}; then \
	    ${INSTALL} -m 755 ${PROG_COV} ${DESTDIR}${prefix}/bin/; fi
	${INSTALL} -m 755 ${PROG_DEBUG} ${DESTDIR}${prefix}/bin/

lldb_ikc3: debug
	if [ -f ${PROG_DEBUG}.core ]; then lldb ${PROG_DEBUG} ${PROG_DEBUG}.core; else lldb ${PROG_DEBUG}; fi

uninstall:
	rm -f ${prefix}/bin/ikc3

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	gdb_ikc3 \
	install \
	lldb_ikc3 \
	update_sources \
	uninstall
