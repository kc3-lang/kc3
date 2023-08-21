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

CLEANFILES = \
	*.a \
	*.css \
	*.gcno \
	*.html \
	*.lo \
	*.o \
	.libs \
	ic3/*.diff \
	ic3/*.err \
	ic3/*.out \
	ic3/*.ret \
	libc3_test \
	libc3_test_asan \
	libc3_test_cov \
	libc3_test_debug

CLEANFILES_COV = \
	*.css \
	*.gcda \
	*.html \
	.libs/*.gcda

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: libc3_test

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: libc3_test_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov: libc3_test_cov

debug: libc3_test_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details test.html

gdb_test: debug
	if [ -f libc3_test_debug.core ]; then gdb .libs/libc3_test_debug libc3_test_debug.core; else gdb .libs/libc3_test_debug; fi

ic3_test_cov:
	IC3=${SRC_TOP}/ic3/ic3_cov time ./ic3_test

lldb_test: debug
	if [ -f libc3_test_debug.core ]; then lldb .libs/libc3_test_debug libc3_test_debug.core; else lldb .libs/libc3_test_debug; fi

test: libc3_test
	time ./libc3_test
	IC3=${SRC_TOP}/ic3/ic3 time ./ic3_test

test_asan: libc3_test_asan
	time ./libc3_test_asan
	IC3=${SRC_TOP}/ic3/ic3_asan time ./ic3_test

test_cov:
	time ./libc3_test_cov
	IC3=${SRC_TOP}/ic3/ic3_cov time ./ic3_test

test_debug: libc3_test_debug
	time ./libc3_test_debug
	IC3=${SRC_TOP}/ic3/ic3_debug time ./ic3_test

test_ic3:
	IC3=${SRC_TOP}/ic3/ic3 time ./ic3_test

test_ic3_debug:
	IC3=${SRC_TOP}/ic3/ic3_debug time ./ic3_test

test_libc3: libc3_test
	time ./libc3_test

test_libc3_cov: libc3_test_cov
	time ./libc3_test_cov

test_valgrind: libc3_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./libc3_test

.PHONY: all asan cov debug clean clean_cov distclean ic3_test_cov libc3_test_cov test test_asan test_cov test_debug test_ic3 test_valgrind

include config.mk
include sources.mk
