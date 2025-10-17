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
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan

clean:
	rm -rf ${CLEANFILES}
	${MAKE} -C demo clean

clean_cov:
	rm -rf ${CLEANFILES_COV}
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
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/window/cairo/xcb
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/window/cairo/xcb; \
	done
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB_DEBUG} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --finish ${DESTDIR}${prefix}/lib
	${MAKE} -C demo install

test: build
	${MAKE} -C demo test

update_sources:
	./update_sources

uninstall:
	rm -rf ${prefix}/include/libkc3/window/cairo/xcb
	rm -f ${prefix}/lib/${LIB}
	${MAKE} -C demo uninstall

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
	gen \
	install \
	lldb_demo \
	test \
	update_sources

include config.mk
include sources.mk
