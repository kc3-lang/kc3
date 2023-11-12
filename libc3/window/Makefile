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
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb build; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo build; fi

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb asan; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo asan; fi

clean:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb clean; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo clean; fi

clean_cov:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb clean_cov; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo clean_cov; fi

cov:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb cov; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo cov; fi

debug:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb debug; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo debug; fi

demo:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo demo; fi

distclean:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb distclean; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo distclean; fi

gcovr:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb gcovr; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo gcovr; fi

gdb_demo: debug
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo gdb_demo; fi

install:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb install; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo install; fi

test:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb test; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo test; fi

update_sources:
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb update_sources; fi
	if ${HAVE_CAIRO} && ${HAVE_XCB}; then ${MAKE} -C cairo/xcb/demo update_sources; fi

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
