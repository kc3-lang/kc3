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

CLEANFILES = *.a ${PROG} ${PROG_ASAN} ${PROG_COV} ${PROG_DEBUG} *.css \
	*.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build:
	${MAKE} ${PROG}

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	${MAKE} ${PROG_ASAN}

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${PROG_COV}

debug:
	${MAKE} ${PROG_DEBUG}

demo:
	${MAKE} ${PROG}
	time ./${PROG}

demo_asan:
	${MAKE} ${PROG_ASAN}
	time ./${PROG_ASAN}

demo_cov:
	${MAKE} ${PROG_COV}
	time ./${PROG_COV}

demo_debug:
	${MAKE} ${PROG_DEBUG}
	time ./${PROG_DEBUG}

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb .libs/${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb .libs/${PROG_DEBUG}; fi

install:
	${INSTALL} -m 755 -d ${DESTDIR}${prefix}/bin
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0755 \
	    ${PROG} ${DESTDIR}${prefix}/bin
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} \
	    -o ${OWNER} -g ${GROUP} -m 0755 \
	    ${PROG_DEBUG} ${DESTDIR}${prefix}/bin

lldb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then lldb .libs/${PROG_DEBUG} ${PROG_DEBUG}.core; else lldb .libs/${PROG_DEBUG}; fi

update_sources:
	./update_sources

uninstall:
	rm -f ${prefix}/bin/${PROG}

.PHONY: \
	all \
	asan \
	build \
	clean \
	clean_cov \
	cov \
	debug \
	demo \
	demo_asan \
	demo_cov \
	demo_debug \
	distclean \
	gdb_demo \
	install \
	lldb_demo \
	update_sources \
	uninstall

include config.mk
include sources.mk
