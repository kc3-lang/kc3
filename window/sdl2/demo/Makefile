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

CLEANFILES_COV = *.css *.gcda *.html
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build:
	${MAKE} ${PROG}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos build; fi

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	${MAKE} ${PROG_ASAN}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos asan; fi

clean:
	rm -rf ${CLEANFILES}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos clean; fi

clean_cov:
	rm -rf ${CLEANFILES_COV}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos clean_cov; fi

cov:
	${MAKE} ${PROG_COV}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos cov; fi

debug:
	${MAKE} ${PROG_DEBUG}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos debug; fi

demo:
	${MAKE} ${PROG}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos demo; else time ./${PROG}; fi

demo_asan:
	${MAKE} asan
	if ${HAVE_DARWIN}; then ${MAKE} -C macos demo_asan; else time ./${PROG_ASAN}; fi

demo_cov:
	${MAKE} cov
	if ${HAVE_DARWIN}; then ${MAKE} -C macos demo_cov; else time ./${PROG_COV}; fi

demo_debug:
	${MAKE} debug
	if ${HAVE_DARWIN}; then ${MAKE} -C macos demo_debug; else time ./${PROG_DEBUG}; fi

distclean:
	rm -rf ${DISTCLEANFILES}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos distclean; fi

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html
	if ${HAVE_DARWIN}; then ${MAKE} -C macos gcovr; fi

gdb_demo: debug
	if ${HAVE_DARWIN}; then ${MAKE} -C macos gdb_demo; else gdb ${PROG_DEBUG}; fi

install:
	install -m 755 -d ${DESTDIR}${prefix}/bin
	install -m 755 ${PROG} ${DESTDIR}${prefix}/bin/${PROG}
	if ${HAVE_DARWIN}; then ${MAKE} -C macos install; fi

lldb_demo: debug
	if ${HAVE_DARWIN}; then ${MAKE} -C macos lldb_demo; else lldb ${PROG_DEBUG}; fi

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
