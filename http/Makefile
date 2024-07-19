## kc3
## Copyright 2022-2024 kmx.io <contact@kmx.io>
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

DISTCLEANFILES = .build ${CLEANFILES} config.h config.mk

all:
	${MAKE} build
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi
	if ${HAVE_GCOV}; then ${MAKE} cov; fi

asan:
	${MAKE} ${LIB_ASAN}

build:
	${MAKE} ${LIB}

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${LIB_COV}

debug:
	${MAKE} ${LIB_DEBUG}

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details libkc3_web.html

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libkc3/ekc3
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libkc3/ekc3
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB_DEBUG} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib

test:

update_sources:
	./update_sources

.PHONY: \
	all \
	asan \
	build \
	clean \
	cov \
	debug \
	distclean \
	gen \
	install \
	test \
	update_sources \

include config.mk
include sources.mk
