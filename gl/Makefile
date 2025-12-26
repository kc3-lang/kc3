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

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C pt asan

build:
	${MAKE} ${LIB}
	${MAKE} -C pt build

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C pt cov

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C pt debug

install::
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3/gl
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3/gl; done
	${MAKE} -C pt install

update_sources:
	./update_sources

uninstall::
	rm -rf ${prefix}/include/libkc3/gl
	${MAKE} -C pt uninstall

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	install \
	test \
	update_sources \
	uninstall
