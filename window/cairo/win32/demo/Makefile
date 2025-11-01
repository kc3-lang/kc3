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

include config.mk
include sources.mk

CLEANFILES = *.a ${PROG} ${PROG_ASAN} ${PROG_COV} ${PROG_DEBUG} *.css \
	*.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: ${PROG}

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: ${PROG_ASAN}

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   ${PROG_COV}

debug: ${PROG_DEBUG}

demo: ${PROG}
	time ./${PROG}

demo_asan: ${PROG_ASAN}
	time ./${PROG_ASAN}

demo_cov: ${PROG_COV}
	time ./${PROG_COV}

demo_debug: ${PROG_DEBUG}
	time ./${PROG_DEBUG}

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

install:
	install -m 755 -d ${prefix}/bin
	install -m 755 ${PROG} ${prefix}/bin/${PROG}

.PHONY: \
	all \
	asan \
	clean \
	clean_cov \
	cov \
	debug \
	demo \
	distclean \
	gdb_demo
