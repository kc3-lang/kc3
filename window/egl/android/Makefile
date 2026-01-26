## kc3
## Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C demo asan

build:
	${MAKE} ${LIB}
	${MAKE} -C demo build

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C demo cov

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C demo debug

demo: build
	${MAKE} -C demo demo

demo_debug: debug
	${MAKE} -C demo demo_debug

install::
	${MAKE} -C demo install

uninstall::
	${MAKE} -C demo uninstall

update_sources:
	./update_sources

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	demo \
	install \
	test \
	update_sources \
