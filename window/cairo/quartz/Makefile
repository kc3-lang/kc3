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

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan

clean::
	${MAKE} -C demo clean

clean_cov::
	${MAKE} -C demo clean_cov

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug

demo: build
	${MAKE} -C demo demo

demo_debug: debug
	${MAKE} -C demo demo_debug

distclean::
	${MAKE} -C demo distclean

gdb_demo: debug
	${MAKE} -C demo gdb_demo

install::
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/window/cairo/quartz
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/window/cairo/quartz; done
	${MAKE} -C demo install

lldb_demo: debug
	${MAKE} -C demo lldb_demo

test: build
	${MAKE} -C demo test

update_sources:
	./update_sources

uninstall::
	rm -rf ${prefix}/include/libkc3/window/cairo/quartz
	${MAKE} -C demo uninstall

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	demo \
	gdb_demo \
	gen \
	install \
	test \
	update_sources \
	uninstall
