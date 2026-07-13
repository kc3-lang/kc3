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

build: kc3txt

asan: ${PROG_ASAN}

cov: kc3txt_cov

debug: ${PROG_DEBUG}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3txt.html

install:
	install -m 755 -d ${DESTDIR}${prefix}/bin
	${INSTALL} -m 755 ${PROG} ${DESTDIR}${prefix}/bin/${PROG}
	if ${HAVE_ASAN}; then \
	    ${INSTALL} -m 755 ${PROG_ASAN} ${DESTDIR}${prefix}/bin/; fi
	if ${HAVE_GCOV}; then \
	    ${INSTALL} -m 755 ${PROG_COV} ${DESTDIR}${prefix}/bin/; fi
	${INSTALL} -m 755 ${PROG_DEBUG} ${DESTDIR}${prefix}/bin/

uninstall:
	rm -f ${prefix}/bin/kc3txt

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	install \
	update_sources \
	uninstall
