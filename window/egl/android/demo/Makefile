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

build:
	${MAKE} ${PROG}

asan:
	${MAKE} ${PROG_ASAN}

cov:
	${MAKE} ${PROG_COV}

debug:
	${MAKE} ${PROG_DEBUG}

demo:
	${MAKE} ${PROG}

demo_asan:
	${MAKE} asan

demo_cov:
	${MAKE} cov

demo_debug:
	${MAKE} debug

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	gdb ${PROG_DEBUG}

install::
	install -m 755 -d ${DESTDIR}${prefix}/bin
	install -m 755 ${PROG} ${DESTDIR}${prefix}/bin

lldb_demo: debug
	lldb ${PROG_DEBUG}

update_sources:
	./update_sources

uninstall::
	rm -f ${prefix}/bin/${PROG}

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	demo \
	demo_asan \
	demo_cov \
	demo_debug \
	gdb_demo \
	install \
	lldb_demo \
	update_sources \
	uninstall
