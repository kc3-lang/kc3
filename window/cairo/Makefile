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

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan
	if ${HAVE_COCOA}; then ${MAKE} -C quartz asan; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 asan; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb asan; fi

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build
	if ${HAVE_COCOA}; then ${MAKE} -C quartz build; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 build; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb build; fi

clean:
	rm -rf ${CLEANFILES}
	${MAKE} -C demo clean
	if ${HAVE_COCOA}; then ${MAKE} -C quartz clean; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 clean; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean; fi

clean_cov:
	${MAKE} -C demo clean_cov
	if ${HAVE_COCOA}; then ${MAKE} -C quartz clean_cov; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 clean_cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov
	if ${HAVE_COCOA}; then ${MAKE} -C quartz cov; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug
	if ${HAVE_COCOA}; then ${MAKE} -C quartz debug; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 debug; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb debug; fi

demo: build
	if ${HAVE_COCOA}; then ${MAKE} -C quartz demo; else if ${HAVE_WIN32}; then ${MAKE} -C win32 demo; else if ${HAVE_XCB}; then ${MAKE} -C xcb demo; fi; fi; fi

demo_asan: asan
	if ${HAVE_COCOA}; then ${MAKE} -C quartz demo_asan; else if ${HAVE_WIN32}; then ${MAKE} -C win32 demo_asan; else if ${HAVE_XCB}; then ${MAKE} -C xcb demo_asan; fi; fi; fi

demo_cov: cov
	if ${HAVE_COCOA}; then ${MAKE} -C quartz demo_cov; else if ${HAVE_WIN32}; then ${MAKE} -C win32 demo_cov; else if ${HAVE_XCB}; then ${MAKE} -C xcb demo_cov; fi; fi; fi

demo_debug: debug
	if ${HAVE_COCOA}; then ${MAKE} -C quartz demo_debug; else if ${HAVE_WIN32}; then ${MAKE} -C win32 demo_debug; else if ${HAVE_XCB}; then ${MAKE} -C xcb demo_debug; fi; fi; fi

distclean:
	rm -rf ${DISTCLEANFILES}
	${MAKE} -C demo distclean
	if ${HAVE_COCOA}; then ${MAKE} -C quartz distclean; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 distclean; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb distclean; fi

gcovr:
	${MAKE} -C demo gcovr
	if ${HAVE_COCOA}; then ${MAKE} -C quartz gcovr; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 gcovr; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb gcovr; fi

gdb_demo: debug
	if ${HAVE_COCOA}; then ${MAKE} -C quartz gdb_demo; else if ${HAVE_WIN32}; then ${MAKE} -C win32 gdb_demo; else if ${HAVE_XCB}; then ${MAKE} -C xcb gdb_demo; fi; fi; fi

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/window/cairo
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/window/cairo; done
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB_DEBUG} ${DESTDIR}${prefix}/lib
	${LIBTOOL} --finish ${DESTDIR}${prefix}/lib
	if ${HAVE_COCOA}; then ${MAKE} -C quartz install; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 install; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb install; fi

lldb_demo: debug
	if ${HAVE_COCOA}; then ${MAKE} -C quartz lldb_demo; else if ${HAVE_WIN32}; then ${MAKE} -C win32 lldb_demo; else if ${HAVE_XCB}; then ${MAKE} -C xcb lldb_demo; fi; fi; fi

test:
	${MAKE} -C demo test
	if ${HAVE_COCOA}; then ${MAKE} -C quartz test; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 test; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb test; fi

update_sources:
	./update_sources

uninstall:
	rm -rf ${prefix}/include/libkc3/window/cairo
	rm -f ${prefix}/lib/${LIB}
	if ${HAVE_COCOA}; then ${MAKE} -C quartz uninstall; fi
	if ${HAVE_WIN32}; then ${MAKE} -C win32 uninstall; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb uninstall; fi

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
