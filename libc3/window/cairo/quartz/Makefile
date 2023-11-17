## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = *.a *.gcno *.la .libs *.lo *.o

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.h config.mk

build: libc3_window_cairo_quartz.la
	${MAKE} -C demo build

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: libc3_window_cairo_quartz_asan.la
	${MAKE} -C demo asan

clean:
	rm -rf ${CLEANFILES}
	${MAKE} -C demo clean

clean_cov:
	rm -rf ${CLEANFILES_COV}
	${MAKE} -C demo clean_cov

cov: libc3_window_cairo_quartz_cov.la
	${MAKE} -C demo cov

debug: libc3_window_cairo_quartz_debug.la
	${MAKE} -C demo debug

demo: build
	${MAKE} -C demo demo

distclean:
	rm -rf ${DISTCLEANFILES}
	${MAKE} -C demo distclean

gdb_demo: debug
	${MAKE} -C demo gdb_demo

lldb_demo: debug
	${MAKE} -C demo lldb_demo

test: build
	${MAKE} -C demo test

update_sources:
	./update_sources

.PHONY: \
	all \
	asan \
	build \
	clean \
	cov \
	debug \
	demo \
	distclean \
	gdb_demo \
	gen \
	install \
	test \
	update_sources

include config.mk
include sources.mk
