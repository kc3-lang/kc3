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
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

build:
	${MAKE} ${LIB}
	if ${HAVE_ANDROID}; then ${MAKE} -C android build; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm build; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb build; fi

asan:
	${MAKE} ${LIB_ASAN}
	if ${HAVE_ANDROID}; then ${MAKE} -C android asan; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm asan; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb asan; fi

clean:
	rm -rf ${CLEANFILES}
	if ${HAVE_ANDROID}; then ${MAKE} -C android clean; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm clean; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean; fi

clean_cov:
	rm -rf ${CLEANFILES_COV}
	if ${HAVE_ANDROID}; then ${MAKE} -C android clean_cov; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm clean_cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	if ${HAVE_ANDROID}; then ${MAKE} -C android cov; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	if ${HAVE_ANDROID}; then ${MAKE} -C android debug; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm debug; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb debug; fi

demo: build
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo; else \
	    if ${HAVE_XCB} && [ -n "${DISPLAY}" ]; then ${MAKE} -C xcb demo; else \
	        if ${HAVE_DRM}; then ${MAKE} -C drm demo; fi; fi; fi

demo_asan: asan
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo_asan; else \
	    if ${HAVE_XCB} && [ -n "${DISPLAY}" ]; then ${MAKE} -C xcb demo_asan; else \
	        if ${HAVE_DRM}; then ${MAKE} -C drm demo_asan; fi; fi; fi

demo_cov: cov
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo_cov; else \
	    if ${HAVE_XCB} && [ -n "${DISPLAY}" ]; then ${MAKE} -C xcb demo_cov; else \
	        if ${HAVE_DRM}; then ${MAKE} -C drm demo_cov; fi; fi; fi

demo_debug: debug
	if ${HAVE_ANDROID}; then ${MAKE} -C android demo_debug; else \
	    if ${HAVE_XCB} && [ -n "${DISPLAY}" ]; then ${MAKE} -C xcb demo_debug; else \
	        if ${HAVE_DRM}; then ${MAKE} -C drm demo_debug; fi; fi; fi

distclean:
	rm -rf ${DISTCLEANFILES}

gdb_demo: debug
	if ${HAVE_ANDROID}; then ${MAKE} -C android gdb_demo; fi
	if ${HAVE_DRM}; then ${MAKE} -C drm gdb_demo; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb gdb_demo; fi

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/window/egl
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/window/egl; done
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB_DEBUG} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --finish ${DESTDIR}${prefix}/lib

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
