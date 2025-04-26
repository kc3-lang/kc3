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

CLEANFILES = *.a *.gcno *.la .libs *.lo *.o

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.h config.mk

all:
	${MAKE} build
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi
	if ${HAVE_GCOV}; then ${MAKE} cov; fi

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build
	${MAKE} -C kubz build

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan
	${MAKE} -C kubz asan

clean:
	rm -rf ${CLEANFILES}
	${MAKE} -C demo clean
	${MAKE} -C kubz clean

clean_cov:
	${MAKE} -C demo clean_cov
	${MAKE} -C kubz clean_cov

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov
	${MAKE} -C kubz cov

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug
	${MAKE} -C kubz debug

demo: build
	${MAKE} -C demo demo

demo_asan: asan
	${MAKE} -C demo demo_asan

demo_cov: cov
	${MAKE} -C demo demo_cov

demo_debug: debug
	${MAKE} -C demo demo_debug

demo_kubz: build
	${MAKE} -C kubz demo

demo_kubz_debug: debug
	${MAKE} -C kubz demo_debug

distclean:
	rm -rf ${DISTCLEANFILES}
	${MAKE} -C demo distclean
	${MAKE} -C kubz distclean

gcovr:
	${MAKE} -C demo gcovr
	${MAKE} -C kubz gcovr

gdb_demo: debug
	${MAKE} -C demo gdb_demo

gdb_kubz: debug
	${MAKE} -C kubz gdb_kubz

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libkc3/window/sdl2
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libkc3/window/sdl2
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib
	${MAKE} -C demo install
	! test -d kubz || ${MAKE} -C kubz install

kubz: all
	${MAKE} -C kubz all

lldb_demo: debug
	${MAKE} -C demo lldb_demo

test:
	${MAKE} -C demo test

update_sources:
	./update_sources

uninstall:
	rm -rf ${prefix}/include/libkc3/window/sdl2
	rm -f ${prefix}/lib/${LIB}
	${MAKE} -C demo uninstall
	! test -d kubz || ${MAKE} -C kubz uninstall

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
	uninstall

include config.mk
include sources.mk
