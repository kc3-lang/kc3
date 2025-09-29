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

build:
	${MAKE} ${LIB}
	if ${HAVE_XCB}; then ${MAKE} -C xcb build; fi

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	${MAKE} ${LIB_ASAN}
	if ${HAVE_XCB}; then ${MAKE} -C xcb asan; fi

clean:
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean; fi

clean_cov:
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	if ${HAVE_XCB}; then ${MAKE} -C xcb cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	if ${HAVE_XCB}; then ${MAKE} -C xcb debug; fi

demo: build

dist:
	${MAKE} -C xcb dist

gdb_demo: debug

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libkc3/window/egl
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libkc3/window/egl
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
	gdb_demo \
	install \
	lldb_demo \
	test \
	update_sources \

include ${TOP}/config.mk
include sources.mk