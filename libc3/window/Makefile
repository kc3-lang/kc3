## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
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
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo build; fi

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	${MAKE} ${LIB_ASAN}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo asan; fi

clean:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo clean; fi

clean_cov:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo clean_cov; fi

cov:
	${MAKE} ${LIB_COV}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo cov; fi

debug:
	${MAKE} ${LIB_DEBUG}
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo debug; fi

demo: build
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo; fi

demo_debug: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo demo_debug; fi

distclean:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo distclean; fi

gcovr:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo gcovr; fi

gdb_demo: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo gdb_demo; fi

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libc3/window
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libc3/window
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo install; fi

lldb_demo: debug
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo lldb_demo; fi

test:
	if ${HAVE_CAIRO}; then ${MAKE} -C cairo test; fi

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
	distclean \
	install \
	test \
	update_sources

include config.mk
include sources.mk
