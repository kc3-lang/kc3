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
	ekc3/*.diff \
	ekc3/*.err \
	ekc3/*.out \
	ekc3/*.ret \
	*.gcno \
	*.html \
	*.lo \
	*.o \
	.libs \
	ikc3/*.diff \
	ikc3/*.err \
	ikc3/*.out \
	ikc3/*.ret \
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

gdb_test_ekc3:
	cd ekc3 && gdb ../../kc3s/.libs/kc3s_debug

gdb_test_http:
	cd http && gdb ../../kc3s/.libs/kc3s_debug

lldb_test: debug
	if [ -f libkc3_test_debug.core ]; then lldb .libs/libkc3_test_debug libkc3_test_debug.core; else lldb .libs/libkc3_test_debug; fi

test: libkc3_test
	time ./libkc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./ekc3_test

test_asan: libkc3_test_asan
	time ./libkc3_test_asan
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./ekc3_test

test_cov:
	time ./libkc3_test_cov
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./ekc3_test

test_debug: libkc3_test_debug
	time ./libkc3_test_debug
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./ekc3_test

test_ekc3:
	KC3S=${SRC_TOP}/kc3s/kc3s time ./ekc3_test

test_ekc3_asan:
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./ekc3_test

test_ekc3_cov:
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./ekc3_test

test_ekc3_debug:
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./ekc3_test

test_http:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./http_test

test_http_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./http_test

test_http_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./http_test

test_http_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./http_test

test_ikc3:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./ikc3_test

test_ikc3_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./ikc3_test

test_ikc3_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./ikc3_test

test_ikc3_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./ikc3_test

test_libkc3: libkc3_test
	time ./libkc3_test

test_libkc3_cov: libkc3_test_cov
	time ./libkc3_test_cov

test_valgrind: libkc3_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./libkc3_test

.PHONY: all asan cov debug clean clean_cov distclean ikc3_test_cov libkc3_test_cov test test_asan test_cov test_debug test_ikc3 test_valgrind

include config.mk
include sources.mk
