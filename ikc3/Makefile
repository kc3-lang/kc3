## kc3
## Copyright 2022-2024 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = *.a ikc3 ikc3_asan ikc3_cov ikc3_debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: ikc3

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: ikc3_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   ikc3_cov

debug: ikc3_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ikc3.html

gdb_ikc3: debug
	if [ -f ikc3_debug.core ]; then gdb .libs/ikc3_debug ikc3_debug.core; else gdb .libs/ikc3_debug; fi

gdb_ikc3_asan: asan
	if [ -f ikc3_debug.core ]; then gdb .libs/ikc3_asan ikc3_asan.core; else gdb .libs/ikc3_asan; fi

install:
	mkdir -p ${prefix}/bin
	install -m 755 ikc3 ${prefix}/bin/ikc3

lldb_ikc3: debug
	if [ -f ikc3_debug.core ]; then lldb .libs/ikc3_debug ikc3_debug.core; else lldb .libs/ikc3_debug; fi

.PHONY: \
	all \
	asan \
	clean \
	clean_cov \
	cov \
	debug \
	distclean \
	gdb_ikc3 \
	install \
	lldb_ikc3 \
	update_sources \

include config.mk
include sources.mk
