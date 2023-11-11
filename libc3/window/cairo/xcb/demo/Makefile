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

CLEANFILES = *.a c3_window_cairo_xcb_demo c3_window_cairo_xcb_demo_asan c3_window_cairo_xcb_demo_cov c3_window_cairo_xcb_demo_debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: c3_window_cairo_xcb_demo

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: c3_window_cairo_xcb_demo_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   c3_window_cairo_xcb_demo_cov

debug: c3_window_cairo_xcb_demo_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details c3_window_cairo_xcb_demo.html

gdb_c3_window_cairo_xcb_demo: debug
	if [ -f c3_window_cairo_xcb_demo_debug.core ]; then gdb .libs/c3_window_cairo_xcb_demo_debug c3_window_cairo_xcb_demo_debug.core; else gdb .libs/c3_window_cairo_xcb_demo_debug; fi

install:
	mkdir -p ${prefix}/bin
	install -m 755 c3_window_cairo_xcb_demo ${prefix}/bin/c3_window_cairo_xcb_demo

.PHONY: all asan cov debug clean clean_cov distclean

include config.mk
include sources.mk
