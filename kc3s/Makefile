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

CLEANFILES = *.a kc3s kc3s_asan kc3s_cov kc3s_debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

build: kc3s

asan: kc3s_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov: kc3s_cov

debug: kc3s_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3s.html

gdb_kc3s: debug
	if [ -f kc3s_debug.core ]; then gdb kc3s_debug kc3s_debug.core; else gdb kc3s_debug; fi

install:
	mkdir -p ${DESTDIR}${prefix}/bin
	install -m 755 kc3s ${DESTDIR}${prefix}/bin/kc3s

uninstall:
	rm -f ${prefix}/bin/kc3s

.PHONY: all asan cov debug clean clean_cov distclean install uninstall

include config.mk
include sources.mk
