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

CLEANFILES = *.a *.gcno *.la .libs *.lo *.o

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.h config.mk

build:
	${MAKE} ${LIB}
	${MAKE} -C window build

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi
	${MAKE} -C window all

asan:
	${MAKE} ${LIB_ASAN}
	${MAKE} -C window asan

clean:
	rm -rf ${CLEANFILES}
	${MAKE} -C window clean

clean_cov:
	rm -rf ${CLEANFILES_COV}
	${MAKE} -C window clean_cov

cov:
	${MAKE} ${LIB_COV}
	${MAKE} -C window cov

debug:
	${MAKE} ${LIB_DEBUG}
	${MAKE} -C window debug

demo: build
	${MAKE} -C window demo

distclean:
	rm -rf ${DISTCLEANFILES}
	${MAKE} -C window distclean

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details libc3.html
	${MAKE} -C window gcovr

gdb_demo: debug
	${MAKE} -C window gdb_demo

include gen.mk
CLEANFILES += ${GENERATED_FILES}

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/libc3
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/libc3
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib
	${LIBTOOL} --finish ${prefix}/lib
	${MAKE} -C window install

libffi:
	ln -sf "../libffi/${CC_TARGET}" libffi

lldb_demo: debug
	${MAKE} -C window lldb_demo

test:

update_sources:
	./update_sources

.PHONY: all asan build clean cov debug distclean gdb_demo gen install lldb_demo test update_sources

include config.mk
include sources.mk
