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
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

build:
	${MAKE} ${LIB}

asan:
	${MAKE} ${LIB_ASAN}

cov:
	${MAKE} ${LIB_COV}

debug:
	${MAKE} ${LIB_DEBUG}

demo: build
	./${PROG}

demo_asan: asan
	./${PROG_ASAN}

demo_cov: cov
	./${PROG_COV}

demo_debug: debug
	./${PROG_DEBUG}

gdb_demo: debug
	gdb ./.libs/${PROG_DEBUG}

lldb_demo: debug
	lldb ./.libs/${PROG_DEBUG}

test:

update_sources:
	./update_sources

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	demo \
	gdb_demo \
	install \
	lldb_demo \
	test \
	update_sources \
