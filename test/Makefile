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

CLEANFILES = \
	*.a \
	*.css \
	ekc3/*.diff \
	ekc3/*.err \
	ekc3/*.out \
	ekc3/*.ret \
	*.gcno \
	*.html \
	http/*.diff \
	http/*.err \
	http/*.out \
	http/*.ret \
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
	if [ -f libkc3_test_debug.core ]; then ${GDB} .libs/libkc3_test_debug libkc3_test_debug.core; else ${GDB} .libs/libkc3_test_debug; fi

gdb_test_ekc3:
	cd ekc3 && ${GDB} ../../kc3s/.libs/kc3s_debug

gdb_test_http:
	cd http && ${GDB} ../../ikc3/.libs/ikc3_debug

gdb_test_http_asan:
	cd http && ${GDB} ../../ikc3/.libs/ikc3_asan

gdb_test_httpd:
	cd httpd && ${GDB} ../../httpd/.libs/kc3_httpd_debug

gdb_test_httpd_asan:
	cd httpd && ${GDB} ../../httpd/.libs/kc3_httpd_asan

gdb_test_ikc3:
	cd ikc3 && ${GDB} ../../ikc3/.libs/ikc3_debug

gdb_test_ikc3_asan:
	cd ikc3 && ${GDB} ../../ikc3/.libs/ikc3_asan

gdb_test_json:
	cd json && ${GDB} ../../ikc3/.libs/ikc3_debug

gdb_test_json_asan:
	cd json && ${GDB} ../../ikc3/.libs/ikc3_asan

gdb_test_markdown:
	cd markdown && ${GDB} ../../ikc3/.libs/ikc3_debug

gdb_test_markdown_asan:
	cd markdown && ${GDB} ../../ikc3/.libs/ikc3_asan

lldb_test: debug
	if [ -f libkc3_test_debug.core ]; then lldb .libs/libkc3_test_debug libkc3_test_debug.core; else lldb .libs/libkc3_test_debug; fi

test: libkc3_test
	time ./libkc3_test --copy
	time ./libkc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./markdown_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./http_test

test_asan: libkc3_test_asan
	time ./libkc3_test_asan --copy
	time ./libkc3_test_asan
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./markdown_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./http_test

test_cov:
	time ./libkc3_test_cov --copy
	time ./libkc3_test_cov
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./markdown_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./http_test

test_debug: libkc3_test_debug
	time ./libkc3_test_debug --copy
	time ./libkc3_test_debug
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./markdown_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./http_test

test_ekc3:
	KC3S=${SRC_TOP}/kc3s/kc3s time ./ekc3_test

test_ekc3_asan:
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./ekc3_test

test_ekc3_cov:
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./ekc3_test

test_ekc3_debug:
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./ekc3_test

test_http:
	KC3S=${SRC_TOP}/kc3s/kc3s time ./http_test

test_http_asan:
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./http_test

test_http_cov:
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./http_test

test_http_debug:
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./http_test

test_httpd:
	${MAKE} -C httpd main

test_httpd_asan:
	${MAKE} -C httpd asan

test_httpd_cov:
	${MAKE} -C httpd cov

test_httpd_debug:
	${MAKE} -C httpd debug

test_ikc3:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./ikc3_test

test_ikc3_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./ikc3_test

test_ikc3_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./ikc3_test

test_ikc3_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./ikc3_test

test_json:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./json_test

test_json_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./json_test

test_json_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./json_test

test_json_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./json_test

test_kc3s:
	KC3S=${SRC_TOP}/kc3s/kc3s time ./kc3s_test

test_kc3s_asan:
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./kc3s_test

test_kc3s_cov:
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./kc3s_test

test_kc3s_debug:
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./kc3s_test

test_libkc3: libkc3_test
	time ./libkc3_test

test_libkc3_cov: libkc3_test_cov
	time ./libkc3_test_cov

test_markdown:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./markdown_test

test_markdown_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./markdown_test

test_markdown_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./markdown_test

test_markdown_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./markdown_test

test_socket:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./socket_test

test_socket_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./socket_test

test_socket_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./socket_test

test_socket_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./socket_test

test_valgrind: libkc3_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./libkc3_test

.PHONY: all \
	asan \
	cov \
	debug \
	clean \
	clean_cov \
	distclean_test_cov \
	gdb_test \
	gdb_test_ekc3 \
	gdb_test_http \
	gdb_test_http_asan \
	gdb_test_httpd \
	gdb_test_httpd_asan \
	gdb_test_ikc3 \
	gdb_test_json \
	gdb_test_kc3s \
	libkc3_test_cov \
	test \
	test_asan \
	test_cov \
	test_debug \
	test_ekc3 \
	test_httpd \
	test_httpd_asan \
	test_httpd_assets \
	test_httpd_cov \
	test_httpd_debug \
	test_ikc3 \
	test_kc3s \
	test_socket \
	test_valgrind

include config.mk
include sources.mk
