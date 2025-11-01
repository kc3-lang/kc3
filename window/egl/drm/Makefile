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

CLEANFILES = *.a *.gcno *.o *.so

CLEANFILES_COV = *.css *.gcda *.html

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

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

dist:
	mkdir -p ${DIST_DIR}
	tar czf ${DIST_DIR}/${TARGET}_${VERSION}.tar.gz LICENSE ${DIST_SRC}

gdb_demo: debug
	${MAKE} -C demo gdb_demo

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libkc3/window/egl/xcb
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libkc3/window/egl/xcb
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB_DEBUG} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib

lldb_demo: debug

test:

update_sources:
	./update_sources

.PHONY: \
	all \
	asan \
	build \
	clean \
	clean_cov \
	cov \
	debug \
	demo \
	dist \
	distclean \
	gdb_demo \
	install \
	lldb_demo \
	test \
	update_sources \

include config.mk
include sources.mk
