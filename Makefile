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

all:
	${MAKE} gen
	${MAKE} -C libtommath all
	${MAKE} -C ucd2c all
	${MAKE} -C libkc3 all
	${MAKE} -C ikc3 all
	${MAKE} -C kc3s all
	${MAKE} -C ekc3 all
	${MAKE} -C test all
	${MAKE} -C libkc3_window all

asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C test asan
	${MAKE} -C libkc3_window asan

build:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c build
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C test build
	${MAKE} -C libkc3_window build

clean:
	${MAKE} -C libtommath clean
	${MAKE} -C ucd2c clean
	${MAKE} -C libkc3 clean
	${MAKE} -C ikc3 clean
	${MAKE} -C kc3s clean
	${MAKE} -C ekc3 clean
	${MAKE} -C test clean
	${MAKE} -C libkc3_window clean

clean_cov:
	${MAKE} -C libtommath clean_cov
	${MAKE} -C libkc3 clean_cov
	${MAKE} -C ikc3 clean_cov
	${MAKE} -C kc3s clean_cov
	${MAKE} -C ekc3 clean_cov
	${MAKE} -C test clean_cov
	${MAKE} -C libkc3_window clean_cov

cov:
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov
	${MAKE} -C test cov
	${MAKE} -C libkc3_window cov

debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C test debug
	${MAKE} -C libkc3_window debug

demo: build
	${MAKE} -C libkc3_window demo

demo_asan: asan
	${MAKE} -C libkc3_window demo_asan

demo_cov: cov
	${MAKE} -C libkc3_window demo_cov

demo_debug: debug
	${MAKE} -C libkc3_window demo_debug

demo_gl: build
	${MAKE} -C libkc3_window demo_gl

demo_gl_asan: asan
	${MAKE} -C libkc3_window demo_gl_asan

demo_gl_cov: cov
	${MAKE} -C libkc3_window demo_gl_cov

demo_gl_debug: debug
	${MAKE} -C libkc3_window demo_gl_debug

dist: kc3-${KC3_VERSION}.tar.gz

kc3-${KC3_VERSION}.tar.gz: kc3.index
	rm -rf kc3-${KC3_VERSION}.old
	-mv kc3-${KC3_VERSION} kc3-${KC3_VERSION}.old
	mkdir kc3-${KC3_VERSION}
	pax -rw < kc3.index kc3-${KC3_VERSION}
	pax -wz kc3-${KC3_VERSION} > kc3-${KC3_VERSION}.tar.gz

kc3.index: sources.mk Makefile
	for F in ${KC3_CONFIGURES} ${KC3_MAKEFILES} ${KC3_C_SOURCES} ${KC3_OBJC_SOURCES} ${KC3_OTHER_SOURCES} ${KC3_EXTERNAL_SOURCES}; do echo "$$F"; done | sort -u > kc3.index

distclean:
	${MAKE} -C libtommath distclean
	${MAKE} -C ucd2c distclean
	${MAKE} -C libkc3 distclean
	${MAKE} -C ikc3 distclean
	${MAKE} -C kc3s distclean
	${MAKE} -C ekc3 distclean
	${MAKE} -C test distclean
	${MAKE} -C libkc3_window distclean

gcovr:
	${MAKE} -C libkc3 gcovr
	${MAKE} -C ikc3 gcovr
	${MAKE} -C kc3s gcovr
	${MAKE} -C ekc3 gcovr
	${MAKE} -C test gcovr
	${MAKE} -C libkc3_window gcovr
	if [ -d "$$HOME/Downloads/kc3_gcovr" ]; then bin/gcovr-to-downloads; fi

gdb_demo:
	${MAKE} debug
	${MAKE} -C libkc3 gdb_demo

gdb_demo_gl:
	${MAKE} debug
	${MAKE} -C libkc3 gdb_demo_gl

gdb_ikc3:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 gdb_ikc3

gdb_test:
	${MAKE} debug
	${MAKE} -C test gdb_test

gen:
	${MAKE} -C libkc3 gen

ikc3:
	${MAKE} -C ikc3 build

ikc3_asan:
	${MAKE} -C ikc3 asan

ikc3_debug:
	${MAKE} -C ikc3 debug

ikc3_gcovr:
	${MAKE} clean_cov
	${MAKE} ikc3_test_cov
	${MAKE} gcovr

install:
	${MAKE} -C libkc3 install
	${MAKE} -C ikc3 install
	${MAKE} -C kc3s install
	${MAKE} -C ekc3 install
	${MAKE} -C libkc3_window install

libkc3_gcovr:
	${MAKE} clean_cov
	${MAKE} libkc3_test_cov
	${MAKE} gcovr

license:
	update_header config.subr ${KC3_CONFIGURES}
	update_header Makefile ${KC3_MAKEFILES}
	update_header libkc3/license.c ${KC3_C_SOURCES}

lldb_demo:
	${MAKE} debug
	${MAKE} -C libkc3 lldb_demo

lldb_demo_gl:
	${MAKE} debug
	${MAKE} -C libkc3 lldb_demo_gl

lldb_ikc3:
	${MAKE} debug
	${MAKE} -C ikc3 lldb_ikc3

lldb_test:
	${MAKE} debug
	${MAKE} -C test lldb_test

test:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C test test
	if ${HAVE_ASAN}; then ${MAKE} test_asan; fi

test_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C test test_asan

test_cov: cov clean_cov
	${MAKE} -C libtommath cov clean_cov
	${MAKE} -C libkc3 cov clean_cov
	${MAKE} -C ikc3 cov clean_cov
	${MAKE} -C kc3s cov clean_cov
	${MAKE} -C ekc3 cov clean_cov
	${MAKE} -C test test_cov

test_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C test test_debug

test_ekc3: build
	${MAKE} -C test test_ekc3

test_ekc3_asan: asan
	${MAKE} -C test test_ekc3_asan

test_ekc3_cov: cov
	${MAKE} -C test test_ekc3_cov

test_ekc3_debug: debug
	${MAKE} -C test test_ekc3_debug

test_gcovr:
	${MAKE} clean_cov
	${MAKE} test_libkc3_cov
	${MAKE} test_ikc3_cov
	${MAKE} test_ekc3_cov
	${MAKE} gcovr

test_ikc3: build
	${MAKE} -C test test_ikc3

test_ikc3_asan: asan
	${MAKE} -C test test_ikc3_asan

test_ikc3_cov: cov
	${MAKE} -C test test_ikc3_cov

test_ikc3_debug: debug
	${MAKE} -C test test_ikc3_debug

test_libkc3: build
	${MAKE} -C test test_libkc3

test_libkc3_cov: cov
	${MAKE} -C test test_libkc3_cov

test_libkc3_debug:
	${MAKE} debug
	${MAKE} -C test test_libkc3_debug

.PHONY: \
	all \
	asan \
	build \
	kc3.index \
	kc3s \
	cov \
	clean \
	clean_cov \
	debug \
	demo \
	demo_asan \
	demo_cov \
	demo_debug \
	demo_gl \
	demo_gl_asan \
	demo_gl_cov \
	demo_gl_debug \
	dist \
	gcovr \
	ikc3 \
	install \
	libkc3 \
	libtommath \
	license \
	test \
	test_asan \
	test_cov \
	test_debug \
	test_gcovr \
	test_ikc3

include config.mk
include sources.mk
