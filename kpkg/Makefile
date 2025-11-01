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

asan: ${PROG_ASAN}

build: ${PROG}

clean_dump:
	rm -f ${SRC_TOP}/lib/kc3/0.1/kpkg.dump

cov:   ${PROG_COV}

debug: ${PROG_DEBUG}

dump: clean_dump
	${SRC_TOP}/kc3s/kc3s --trace --load kpkg.kc3 --dump ${SRC_TOP}/lib/kc3/0.1/kpkg.dump --quit
	time ${SRC_TOP}/kc3s/kc3s --trace --restore ${SRC_TOP}/lib/kc3/0.1/kpkg.dump --quit

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

install:
	mkdir -p ${DESTDIR}${prefix}/bin
	${INSTALL} -m 755 ${PROG} ${DESTDIR}${prefix}/bin/${PROG}

lldb_${PROG}: debug
	if [ -f ${PROG_DEBUG}.core ]; then lldb ${PROG_DEBUG} ${PROG_DEBUG}.core; else lldb ${PROG_DEBUG}; fi

uninstall:
	rm -f ${prefix}/bin/${PROG}

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	gdb \
	gdb_asan \
	install \
	lldb \
	lldb_asan \
	update_sources \
	uninstall
