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

build: ${LIB}
build:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz build; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb build; fi

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: ${LIB_ASAN}
asan:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz asan; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb asan; fi

clean:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz clean; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean; fi

clean_cov:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz clean_cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb clean_cov; fi

cov: ${LIB_COV}
cov:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz cov; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb cov; fi

debug: ${LIB_DEBUG}
debug:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz debug; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb debug; fi

demo: build
demo:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz demo; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb demo; fi

distclean:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz distclean; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb distclean; fi

gcovr:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz gcovr; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb gcovr; fi

gdb_demo: debug
gdb_demo:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz gdb_demo; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb gdb_demo; fi

install:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz install; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb install; fi

test:
	if ${HAVE_SWIFT}; then ${MAKE} -C quartz test; fi
	if ${HAVE_XCB}; then ${MAKE} -C xcb test; fi

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
	gdb_demo \
	install \
	test \
	update_sources

include config.mk
