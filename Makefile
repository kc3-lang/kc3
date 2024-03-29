## c3
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
	${MAKE} -C libc3 all
	${MAKE} -C ic3 all
	${MAKE} -C c3s all
	${MAKE} -C test all
	${MAKE} -C libc3_window all

asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C libc3 asan
	${MAKE} -C ic3 asan
	${MAKE} -C c3s asan
	${MAKE} -C test asan
	${MAKE} -C libc3_window asan

build:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c build
	${MAKE} -C libc3 build
	${MAKE} -C ic3 build
	${MAKE} -C c3s build
	${MAKE} -C test build
	${MAKE} -C libc3_window build

clean:
	${MAKE} -C libtommath clean
	${MAKE} -C ucd2c clean
	${MAKE} -C libc3 clean
	${MAKE} -C ic3 clean
	${MAKE} -C c3s clean
	${MAKE} -C test clean
	${MAKE} -C libc3_window clean

clean_cov:
	${MAKE} -C libtommath clean_cov
	${MAKE} -C libc3 clean_cov
	${MAKE} -C ic3 clean_cov
	${MAKE} -C c3s clean_cov
	${MAKE} -C test clean_cov
	${MAKE} -C libc3_window clean_cov

cov:
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C libc3 cov
	${MAKE} -C ic3 cov
	${MAKE} -C c3s cov
	${MAKE} -C test cov
	${MAKE} -C libc3_window cov

debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libc3 debug
	${MAKE} -C ic3 debug
	${MAKE} -C c3s debug
	${MAKE} -C test debug
	${MAKE} -C libc3_window debug

demo: build
	${MAKE} -C libc3_window demo

demo_asan: asan
	${MAKE} -C libc3_window demo_asan

demo_cov: cov
	${MAKE} -C libc3_window demo_cov

demo_debug: debug
	${MAKE} -C libc3_window demo_debug

demo_gl: build
	${MAKE} -C libc3_window demo_gl

demo_gl_asan: asan
	${MAKE} -C libc3_window demo_gl_asan

demo_gl_cov: cov
	${MAKE} -C libc3_window demo_gl_cov

demo_gl_debug: debug
	${MAKE} -C libc3_window demo_gl_debug

dist: c3-${C3_VERSION}.tar.gz

c3-${C3_VERSION}.tar.gz: c3.index
	rm -rf c3-${C3_VERSION}.old
	-mv c3-${C3_VERSION} c3-${C3_VERSION}.old
	mkdir c3-${C3_VERSION}
	pax -rw < c3.index c3-${C3_VERSION}
	pax -wz c3-${C3_VERSION} > c3-${C3_VERSION}.tar.gz

c3.index: sources.mk Makefile
	for F in ${C3_CONFIGURES} ${C3_MAKEFILES} ${C3_C_SOURCES} ${C3_OBJC_SOURCES} ${C3_OTHER_SOURCES} ${C3_EXTERNAL_SOURCES}; do echo "$$F"; done | sort -u > c3.index

distclean:
	${MAKE} -C libtommath distclean
	${MAKE} -C ucd2c distclean
	${MAKE} -C libc3 distclean
	${MAKE} -C ic3 distclean
	${MAKE} -C c3s distclean
	${MAKE} -C test distclean
	${MAKE} -C libc3_window distclean

gcovr:
	${MAKE} -C libc3 gcovr
	${MAKE} -C ic3 gcovr
	${MAKE} -C c3s gcovr
	${MAKE} -C test gcovr
	${MAKE} -C libc3_window gcovr
	if [ -d "$$HOME/Downloads/c3_gcovr" ]; then bin/gcovr-to-downloads; fi

gdb_demo:
	${MAKE} debug
	${MAKE} -C libc3 gdb_demo

gdb_demo_gl:
	${MAKE} debug
	${MAKE} -C libc3 gdb_demo_gl

gdb_ic3:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libc3 debug
	${MAKE} -C ic3 gdb_ic3

gdb_test:
	${MAKE} debug
	${MAKE} -C test gdb_test

gen:
	${MAKE} -C libc3 gen

ic3:
	${MAKE} -C ic3 build

ic3_asan:
	${MAKE} -C ic3 asan

ic3_debug:
	${MAKE} -C ic3 debug

ic3_gcovr:
	${MAKE} clean_cov
	${MAKE} ic3_test_cov
	${MAKE} gcovr

install:
	${MAKE} -C libc3 install
	${MAKE} -C ic3 install
	${MAKE} -C c3s install
	${MAKE} -C libc3_window install

libc3_gcovr:
	${MAKE} clean_cov
	${MAKE} libc3_test_cov
	${MAKE} gcovr

license:
	update_header config.subr ${C3_CONFIGURES}
	update_header Makefile ${C3_MAKEFILES}
	update_header libc3/license.c ${C3_C_SOURCES}

lldb_demo:
	${MAKE} debug
	${MAKE} -C libc3 lldb_demo

lldb_demo_gl:
	${MAKE} debug
	${MAKE} -C libc3 lldb_demo_gl

lldb_ic3:
	${MAKE} debug
	${MAKE} -C ic3 lldb_ic3

lldb_test:
	${MAKE} debug
	${MAKE} -C test lldb_test

test: build
test:
	${MAKE} -C test test
	if ${HAVE_ASAN}; then ${MAKE} test_asan; fi

test_asan: asan
test_asan:
	${MAKE} -C test test_asan

test_cov: cov clean_cov
	${MAKE} -C test test_cov

test_debug:
	${MAKE} debug
	${MAKE} -C test test_debug

test_gcovr:
	${MAKE} clean_cov
	${MAKE} test_libc3_cov
	${MAKE} test_ic3_cov
	${MAKE} gcovr

test_ic3: build
	${MAKE} -C test test_ic3

test_ic3_asan: asan
	${MAKE} -C test test_ic3_asan

test_ic3_cov: cov
	${MAKE} -C test test_ic3_cov

test_ic3_debug: debug
	${MAKE} -C test test_ic3_debug

test_libc3: build
	${MAKE} -C test test_libc3

test_libc3_cov: cov
	${MAKE} -C test test_libc3_cov

test_libc3_debug:
	${MAKE} debug
	${MAKE} -C test test_libc3_debug

.PHONY: \
	all \
	asan \
	build \
	c3.index \
	c3s \
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
	ic3 \
	install \
	libc3 \
	libtommath \
	license \
	test \
	test_asan \
	test_cov \
	test_debug \
	test_gcovr \
	test_ic3

include config.mk
include sources.mk
