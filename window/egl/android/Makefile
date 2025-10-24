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

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	${MAKE} ${LIB_ASAN}

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${LIB_COV}

debug:
	${MAKE} ${LIB_DEBUG}

demo: build
	${MAKE} -C demo demo

demo_debug: debug
	${MAKE} -C demo demo_debug

distclean:
	rm -rf ${DISTCLEANFILES}

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
	distclean \
	install \
	test \
	update_sources \

include config.mk
include sources.mk
