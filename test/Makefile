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
	libkc3_test \
	libkc3_test_asan \
	libkc3_test_cov \
	libkc3_test_debug

CLEANFILES_COV = \
	*.css \
	*.gcda \
	*.html \
	.libs/*.gcda

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: libkc3_test

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: libkc3_test_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov: libkc3_test_cov

debug: libkc3_test_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details test.html

gdb_test: debug
	if [ -f libkc3_test_debug.core ]; then gdb .libs/libkc3_test_debug libkc3_test_debug.core; else gdb .libs/libkc3_test_debug; fi

lldb_test: debug
	if [ -f libkc3_test_debug.core ]; then lldb .libs/libkc3_test_debug libkc3_test_debug.core; else lldb .libs/libkc3_test_debug; fi

test: libkc3_test
	time ./libkc3_test
	IC3=${SRC_TOP}/ic3/ic3 time ./ic3_test

test_asan: libkc3_test_asan
	time ./libkc3_test_asan
	IC3=${SRC_TOP}/ic3/ic3_asan time ./ic3_test

test_cov:
	time ./libkc3_test_cov
	IC3=${SRC_TOP}/ic3/ic3_cov time ./ic3_test

test_debug: libkc3_test_debug
	time ./libkc3_test_debug
	IC3=${SRC_TOP}/ic3/ic3_debug time ./ic3_test

test_ic3:
	IC3=${SRC_TOP}/ic3/ic3 time ./ic3_test

test_ic3_asan:
	IC3=${SRC_TOP}/ic3/ic3_asan time ./ic3_test

test_ic3_cov:
	IC3=${SRC_TOP}/ic3/ic3_cov time ./ic3_test

test_ic3_debug:
	IC3=${SRC_TOP}/ic3/ic3_debug time ./ic3_test

test_libkc3: libkc3_test
	time ./libkc3_test

test_libkc3_cov: libkc3_test_cov
	time ./libkc3_test_cov

test_valgrind: libkc3_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./libkc3_test

.PHONY: all asan cov debug clean clean_cov distclean ic3_test_cov libkc3_test_cov test test_asan test_cov test_debug test_ic3 test_valgrind

include config.mk
include sources.mk
