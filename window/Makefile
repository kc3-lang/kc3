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
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi
	if ${HAVE_GCOV}; then ${MAKE} cov; fi

include config.mk
include sources.mk

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo asan; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl asan; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 asan; fi

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo build; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl build; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 build; fi

clean::
	${MAKE} -C demo clean
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo clean; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl clean; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 clean; fi

clean_cov::
	${MAKE} -C demo clean_cov
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo clean_cov; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl clean_cov; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo cov; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl cov; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo debug; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl debug; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 debug; fi

demo: build
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo; fi

demo_asan: asan
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_asan; fi

demo_cov: cov
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_cov; fi

demo_debug: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_debug; fi

demo_egl: build
	if ${HAVE_EGL}; then ${MAKE} -C egl demo; fi

demo_egl_asan: asan
	if ${HAVE_EGL}; then ${MAKE} -C egl demo_asan; fi

demo_egl_cov: cov
	if ${HAVE_EGL}; then ${MAKE} -C egl demo_cov; fi

demo_egl_debug: debug
	if ${HAVE_EGL}; then ${MAKE} -C egl demo_debug; fi

demo_gl: build
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo; fi

demo_gl_asan: asan
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo_asan; fi

demo_gl_cov: cov
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo_cov; fi

demo_gl_debug: debug
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo_debug; fi

distclean::
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo distclean; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 distclean; fi

gcovr:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo gcovr; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 gcovr; fi

gdb_demo: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo gdb_demo; fi

gdb_demo_gl: debug
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 gdb_demo; fi

gdb_kubz: debug
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 gdb_kubz; fi

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/window
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/window; done
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/lib
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB} ${DESTDIR}${prefix}/lib
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB_DEBUG} ${DESTDIR}${prefix}/lib
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo install; fi
	if ${HAVE_EGL}; then ${MAKE} -C egl install; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 install; fi

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
	rm -rf ${prefix}/include/libkc3/window
	rm -f ${prefix}/lib/${LIB}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo uninstall; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 uninstall; fi

.PHONY: \
	all \
	asan \
	build \
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
	gdb_demo \
	gdb_demo_gl \
	install \
	lldb_demo \
	lldb_demo_gl \
	test \
	update_sources \
	uninstall
