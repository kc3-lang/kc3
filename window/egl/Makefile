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
	if ${HAVE_ANDROID}; then ${MAKE} -C android build; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb build; fi

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan
	if ${HAVE_ANDROID}; then ${MAKE} -C android asan; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb asan; fi

clean:
	rm -rf ${CLEANFILES}
	${MAKE} -C demo clean
	if ${HAVE_ANDROID}; then ${MAKE} -C android clean; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean; fi

clean_cov:
	rm -rf ${CLEANFILES_COV}
	${MAKE} -C demo clean_cov
	if ${HAVE_ANDROID}; then ${MAKE} -C android clean_cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov
	if ${HAVE_ANDROID}; then ${MAKE} -C android cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug
	if ${HAVE_ANDROID}; then ${MAKE} -C android debug; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb debug; fi

demo: build
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb demo; fi

demo_asan: asan
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo_asan; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb demo_asan; fi

demo_cov: cov
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo_cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb demo_cov; fi

demo_debug: debug
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo_debug; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb demo_debug; fi

distclean:
	rm -rf ${DISTCLEANFILES}

gdb_demo: debug
	if ${HAVE_ANDROID}; then ${MAKE} -C android gdb_demo; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb gdb_demo; fi

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
	distclean \
	gdb_demo \
	install \
	lldb_demo \
	test \
	update_sources \

include config.mk
include sources.mk
