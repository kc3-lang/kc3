## kc3
## Copyright 2022-2024 kmx.io <contact@kmx.io>
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

asan:
	${MAKE} ${LIB_ASAN}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo asan; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 asan; fi

build:
	${MAKE} ${LIB}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo build; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 build; fi

clean:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo clean; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 clean; fi

clean_cov:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo clean_cov; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo cov; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo debug; fi
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 debug; fi

demo: build
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo; fi

demo_asan: asan
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_asan; fi

demo_cov: cov
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_cov; fi

demo_debug: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_debug; fi

demo_gl: build
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo; fi

demo_gl_asan: asan
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo_asan; fi

demo_gl_cov: cov
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo_cov; fi

demo_gl_debug: debug
	if ${HAVE_SDL2}; then ${MAKE} -C sdl2 demo_debug; fi

distclean:
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
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libkc3/window
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libkc3/window
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo install; fi
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
	update_sources

include config.mk
include sources.mk
