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

build:
	${MAKE} gen
	${GMAKE} -C libffi all
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c build
	${MAKE} -C libc3 build
	${MAKE} -C ic3 build
	${MAKE} -C c3s build
	${MAKE} -C test build

all:
	${MAKE} gen
	${GMAKE} -C libffi all
	${MAKE} -C libtommath all
	${MAKE} -C ucd2c all
	${MAKE} -C libc3 all
	${MAKE} -C ic3 all
	${MAKE} -C c3s all
	${MAKE} -C test all

asan:
	${GMAKE} -C libffi all
	${MAKE} -C libtommath asan
	${MAKE} -C libc3 asan
	${MAKE} -C ic3 asan
	${MAKE} -C c3s asan
	${MAKE} -C test asan

clean:
	${GMAKE} -C libffi clean
	${MAKE} -C libtommath clean
	${MAKE} -C ucd2c clean
	${MAKE} -C libc3 clean
	${MAKE} -C ic3 clean
	${MAKE} -C c3s clean
	${MAKE} -C test clean

clean_cov:
	${MAKE} -C libtommath clean_cov
	${MAKE} -C libc3 clean_cov
	${MAKE} -C ic3 clean_cov
	${MAKE} -C c3s clean_cov
	${MAKE} -C test clean_cov

cov:
	${MAKE} gen
	${GMAKE} -C libffi all
	${MAKE} -C libtommath cov
	${MAKE} -C libc3 cov
	${MAKE} -C ic3 cov
	${MAKE} -C c3s cov
	${MAKE} -C test cov

debug:
	${GMAKE} -C libffi all
	${MAKE} -C libtommath debug
	${MAKE} -C libc3 debug
	${MAKE} -C ic3 debug
	${MAKE} -C c3s debug
	${MAKE} -C test debug

demo: build
	${MAKE} -C libc3 demo

demo_debug: debug
	${MAKE} -C libc3 demo_debug

demo_gl: build
	${MAKE} -C libc3 demo_gl

demo_gl_debug: debug
	${MAKE} -C libc3 demo_gl_debug

dist: c3-${C3_VERSION}.tar.gz

c3-${C3_VERSION}.tar.gz:
	rm -rf c3-${C3_VERSION}.old
	-mv c3-${C3_VERSION} c3-${C3_VERSION}.old
	mkdir c3-${C3_VERSION}
	pax -rw < c3.index c3-${C3_VERSION}
	pax -wz c3-${C3_VERSION} > c3-${C3_VERSION}.tar.gz

distclean:
	${MAKE} -C libtommath distclean
	${MAKE} -C ucd2c distclean
	${MAKE} -C libc3 distclean
	${MAKE} -C ic3 distclean
	${MAKE} -C c3s distclean
	${MAKE} -C test distclean

gcovr:
	${MAKE} -C libc3 gcovr
	${MAKE} -C ic3 gcovr
	${MAKE} -C c3s gcovr
	${MAKE} -C test gcovr
	if [ -d "$$HOME/Downloads/c3_gcovr" ]; then bin/gcovr-to-downloads; fi

gdb_demo:
	${MAKE} debug
	${MAKE} -C libc3 gdb_demo

gdb_demo_gl:
	${MAKE} debug
	${MAKE} -C libc3 gdb_demo_gl

gdb_ic3:
	${MAKE} debug
	${MAKE} -C ic3 gdb_ic3

gdb_test:
	${MAKE} debug
	${MAKE} -C test gdb_test

gen:
	${MAKE} -C libc3 gen

ic3_gcovr:
	${MAKE} clean_cov
	${MAKE} ic3_test_cov
	${MAKE} gcovr

install:
	${GMAKE} -C libffi install
	${MAKE} -C libc3 install
	${MAKE} -C ic3 install
	${MAKE} -C c3s install

libc3_gcovr:
	${MAKE} clean_cov
	${MAKE} libc3_test_cov
	${MAKE} gcovr

libffi:
	${GMAKE} -C libffi all

license:
	update_header config.subr ${C3_CONFIGURES}
	update_header Makefile ${C3_MAKEFILES}
	update_header license.h ${C3_C_SOURCES}

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
test_cov:
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
test_ic3:
	${MAKE} -C test test_ic3

test_ic3_asan: asan
test_ic3_asan:
	${MAKE} -C test test_ic3_asan

test_ic3_cov: cov
test_ic3_cov:
	${MAKE} -C test test_ic3_cov

test_ic3_debug: debug
test_ic3_debug:
	${MAKE} -C test test_ic3_debug

test_libc3: build
test_libc3:
	${MAKE} -C test test_libc3

test_libc3_cov: cov
test_libc3_cov:
	${MAKE} -C test test_libc3_cov

test_libc3_debug:
	${MAKE} debug
	${MAKE} -C test test_libc3_debug

.PHONY: \
	all \
	asan \
	c3s \
	cov \
	clean \
	clean_cov \
	debug \
	demo \
	dist \
	gcovr \
	ic3 \
	install \
	libc3 \
	libtommath \
	libffi \
	license \
	test \
	test_asan \
	test_cov \
	test_debug \
	test_gcovr \
	test_ic3

include config.mk
include sources.mk
