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
	if ${HAVE_ASAN}; then ${MAKE} asan; fi
	if ${HAVE_GCOV}; then ${MAKE} cov; fi

include gen.mk
include config.mk
include sources.mk

asan:
	${MAKE} ${LIB_ASAN}

build:
	${MAKE} ${LIB}

cov:
	${MAKE} ${LIB_COV}

debug:
	${MAKE} ${LIB_DEBUG}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details libkc3.html

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/include/libkc3
	for H in ${HEADERS}; do \
	    ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
		${SRC_DIR}/$$H \
		${DESTDIR}${prefix}/include/libkc3; done
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d \
	    ${DESTDIR}${prefix}/lib
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB}*.${SHARED_EXT}* ${DESTDIR}${prefix}/lib
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 \
	    ${LIB_DEBUG}*.${SHARED_EXT}* ${DESTDIR}${prefix}/lib

test:

update_sources:
	./update_sources

uninstall:
	rm -rf ${prefix}/include/libkc3
	rm -f ${prefix}/lib/${LIB}*.${SHARED_EXT}*
	rm -f ${prefix}/lib/${LIB_DEBUG}*.${SHARED_EXT}*

.PHONY: \
	all \
	asan \
	build \
	cov \
	debug \
	gen \
	install \
	test \
	update_sources
