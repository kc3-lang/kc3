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

DISTCLEANFILES = ${CLEANFILES} config.mk

all:
	${MAKE} build
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi
	if ${HAVE_GCOV}; then ${MAKE} cov; fi

asan:
	${MAKE} ${LIB_ASAN}

build:
	${MAKE} ${LIB}

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${LIB_COV}

debug:
	${MAKE} ${LIB_DEBUG}

distclean:
	rm -rf ${DISTCLEANFILES}

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/gl
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/gl; done
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB_DEBUG} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --finish ${DESTDIR}${prefix}/lib

lldb_demo: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo lldb_demo; fi

lldb_demo_gl: debug
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 lldb_demo; fi

test:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo test; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 test; fi

update_sources:
	./update_sources

uninstall:
	rm -rf ${prefix}/include/libkc3/gl
	rm -f ${prefix}/lib/${LIB}
	rm -f ${prefix}/lib/${LIB_DEBUG}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo uninstall; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 uninstall; fi

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
	demo_gl \
	demo_gl_asan \
	demo_gl_cov \
	demo_gl_debug \
	distclean \
	gdb_demo \
	gdb_demo_gl \
	install \
	lldb_demo \
	lldb_demo_gl \
	test \
	update_sources \
	uninstall

include config.mk
include sources.mk
