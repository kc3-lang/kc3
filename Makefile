## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software excepted
## on Apple computers granted the above copyright notice and
## this permission paragraph are included in all copies and
## substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

build: gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c build
	${MAKE} -C libc3 build
	${MAKE} -C ic3 build
	${MAKE} -C c3s build
	${MAKE} -C test build

all: gen
	${MAKE} -C libtommath all
	${MAKE} -C ucd2c all
	${MAKE} -C libc3 all
	${MAKE} -C ic3 all
	${MAKE} -C c3s all
	${MAKE} -C test all

asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libc3 asan
	${MAKE} -C ic3 asan
	${MAKE} -C c3s asan
	${MAKE} -C test asan

cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libc3 cov
	${MAKE} -C ic3 cov
	${MAKE} -C c3s cov
	${MAKE} -C test cov

clean:
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

debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libc3 debug
	${MAKE} -C ic3 debug
	${MAKE} -C c3s debug
	${MAKE} -C test debug

dist: c3-${C3_VERSION}.tar.gz

c3-${C3_VERSION}.tar.gz:
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

gdb_ic3: debug
	${MAKE} -C ic3 gdb_ic3

gdb_test: debug
	${MAKE} -C test gdb_test

gen:
	${MAKE} -C libc3 gen

ic3_gcovr:
	${MAKE} clean_cov
	${MAKE} ic3_test_cov
	${MAKE} gcovr

ic3_test_cov: cov
	${MAKE} -C test ic3_test_cov

install:
	${MAKE} -C libc3 install
	${MAKE} -C ic3 install
	${MAKE} -C c3s install

libc3_gcovr:
	${MAKE} clean_cov
	${MAKE} libc3_test_cov
	${MAKE} gcovr

libc3_test_cov: cov
	${MAKE} -C test libc3_test_cov

licence:
	update_header config.subr ${C3_CONFIGURES}
	update_header Makefile ${C3_MAKEFILES}
	update_header libc3/c3.h ${C3_C_SOURCES}

test: build
	${MAKE} -C test test

test_asan: asan
	${MAKE} -C test test_asan

test_cov: cov clean_cov
	${MAKE} -C test test_cov

test_debug: debug
	${MAKE} -C test test_debug

test_gcovr:
	${MAKE} clean_cov
	${MAKE} libc3_test_cov
	${MAKE} ic3_test_cov
	${MAKE} gcovr

test_ic3: build
	${MAKE} -C test test_ic3

.PHONY: all asan c3s cov clean clean_cov debug gcovr ic3 install libc3 libtommath license test test_asan test_cov test_debug test_gcovr test_ic3

include config.mk
include sources.mk
