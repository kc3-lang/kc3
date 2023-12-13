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
	${MAKE} ${LIB}
	${MAKE} -C demo build

all:
	${MAKE} build

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan

clean:
	${MAKE} -C demo clean

clean_cov:
	${MAKE} -C demo clean_cov

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug

demo: build
	${MAKE} -C demo demo

demo_asan: asan
	${MAKE} -C demo demo_asan

demo_cov: cov
	${MAKE} -C demo demo_cov

demo_debug: debug
	${MAKE} -C demo demo_debug

distclean:
	rm -rf ${DISTCLEANFILES}
	${MAKE} -C demo distclean

gcovr:
	${MAKE} -C demo gcovr

gdb_demo: debug
	${MAKE} -C demo gdb_demo

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libc3/window/sdl2
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libc3/window/sdl2
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib

lldb_demo: debug
	${MAKE} -C demo lldb_demo

test:
	${MAKE} -C demo test

update_sources:
	./update_sources

.PHONY: \
	all \
	asan \
	build \
	clean \
	cov \
	debug \
	demo \
	demo_asan \
	demo_cov \
	demo_debug \
	distclean \
	gdb_demo \
	install \
	lldb_demo \
	test \
	update_sources \

include config.mk
include sources.mk
