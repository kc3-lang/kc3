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

CLEANFILES += \
	ekc3/*.diff \
	ekc3/*.err \
	ekc3/*.out \
	ekc3/*.ret \
	http/*.diff \
	http/*.err \
	http/*.out \
	http/*.ret \
	ikc3/*.diff \
	ikc3/*.err \
	ikc3/*.out \
	ikc3/*.ret \
	kc3.dump \
	${PROG} \
	libkc3_test_asan \
	libkc3_test_cov \
	libkc3_test_debug

build: ${PROG}

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: ${PROG_ASAN}

cov: ${PROG_COV}

debug: ${PROG_DEBUG}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details test.html

gdb_test: debug
	rm -f kc3.dump
	if [ -f libkc3_test_debug.core ]; then ${GDB} libkc3_test_debug libkc3_test_debug.core; else ${GDB} libkc3_test_debug; fi

gdb_test_asan: asan
	rm -f kc3.dump
	if [ -f libkc3_test_asan.core ]; then ${GDB} libkc3_test_asan libkc3_test_asan.core; else ${GDB} libkc3_test_asan; fi

gdb_test_ekc3:
	cd ekc3 && ${GDB} ../../kc3s/kc3s_debug

gdb_test_http:
	cd http && ${GDB} ../../ikc3/ikc3_debug

gdb_test_http_asan:
	cd http && ${GDB} ../../ikc3/ikc3_asan

gdb_test_httpd:
	cd httpd && ${GDB} ../../httpd/kc3_httpd_debug

gdb_test_httpd_asan:
	cd httpd && ${GDB} ../../httpd/kc3_httpd_asan

gdb_test_ikc3:
	cd ikc3 && ${GDB} ../../ikc3/ikc3_debug

gdb_test_ikc3_asan:
	cd ikc3 && ${GDB} ../../ikc3/ikc3_asan

gdb_test_json:
	cd json && ${GDB} ../../ikc3/ikc3_debug

gdb_test_json_asan:
	cd json && ${GDB} ../../ikc3/ikc3_asan

gdb_test_markdown:
	cd markdown && ${GDB} ../../ikc3/ikc3_debug

gdb_test_markdown_asan:
	cd markdown && ${GDB} ../../ikc3/ikc3_asa

gdb_test_pdf:
	cd pdf && ${GDB} ../../ikc3/ikc3_debug

gdb_test_pdf_asan:
	cd pdf && ${GDB} ../../ikc3/ikc3_asan

gdb_test_tls:
	cd tls && ${GDB} ../../ikc3/ikc3_debug

gdb_test_tls_asan:
	cd tls && ${GDB} ../../ikc3/ikc3_asan

lldb_test: debug
	rm -f kc3.dump
	if [ -f libkc3_test_debug.core ]; then lldb libkc3_test_debug libkc3_test_debug.core; else lldb libkc3_test_debug; fi

lldb_test_tls:
	cd tls && lldb ../../ikc3/ikc3_debug

lldb_test_tls_asan:
	cd tls && lldb ../../ikc3/ikc3_asan

test: ${PROG}
	rm -f kc3.dump
	time ./${PROG}
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./tls_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./pdf_test
	KC3S=${SRC_TOP}/kc3s/kc3s time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./markdown_test
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./http_test

test_asan: ${PROG_ASAN}
	rm -f kc3.dump
	time ./${PROG_ASAN} --copy
	time ./${PROG_ASAN}
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./tls_test
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./markdown_test
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./pdf_test
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./http_test

test_cov:
	rm -f kc3.dump
	time ./libkc3_test_cov
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./tls_test
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./markdown_test
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./pdf_test
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./http_test

test_debug: libkc3_test_debug
	rm -f kc3.dump
	time ./libkc3_test_debug
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./ikc3_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./kc3s_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./tls_test
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./ekc3_test
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./json_test
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./markdown_test
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./pdf_test
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./http_test

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

test_libkc3: ${PROG}
	rm -f kc3.dump
	time ./${PROG}

test_libkc3_asan: ${PROG_ASAN}
	rm -f kc3.dump
	time ./${PROG_ASAN}

test_libkc3_cov: libkc3_test_cov
	rm -f kc3.dump
	time ./libkc3_test_cov

test_libkc3_debug: libkc3_test_debug
	rm -f kc3.dump
	time ./libkc3_test_debug

test_markdown:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./markdown_test

test_markdown_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./markdown_test

test_markdown_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./markdown_test

test_markdown_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./markdown_test

test_pdf:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./pdf_test

test_pdf_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./pdf_test

test_pdf_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./pdf_test

test_pdf_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./pdf_test

test_socket:
	IKC3=${SRC_TOP}/ikc3/ikc3 time ./socket_test

test_socket_asan:
	IKC3=${SRC_TOP}/ikc3/ikc3_asan time ./socket_test

test_socket_cov:
	IKC3=${SRC_TOP}/ikc3/ikc3_cov time ./socket_test

test_socket_debug:
	IKC3=${SRC_TOP}/ikc3/ikc3_debug time ./socket_test

test_tls:
	KC3S=${SRC_TOP}/kc3s/kc3s time ./tls_test

test_tls_asan:
	KC3S=${SRC_TOP}/kc3s/kc3s_asan time ./tls_test

test_tls_cov:
	KC3S=${SRC_TOP}/kc3s/kc3s_cov time ./tls_test

test_tls_debug:
	KC3S=${SRC_TOP}/kc3s/kc3s_debug time ./tls_test

test_valgrind: ${PROG}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./${PROG}

.PHONY: all \
	asan \
	cov \
	debug \
	gdb_test \
	gdb_test_ekc3 \
	gdb_test_http \
	gdb_test_http_asan \
	gdb_test_httpd \
	gdb_test_httpd_asan \
	gdb_test_ikc3 \
	gdb_test_json \
	gdb_test_kc3s \
	gdb_test_tls \
	gdb_test_tls_asan \
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
	test_libkc3 \
	test_libkc3_asan \
	test_libkc3_cov \
	test_libkc3_debug \
	test_socket \
	test_tls \
	test_tls_asan \
	test_tls_cov \
	test_tls_debug \
	test_valgrind

include config.mk
include sources.mk
