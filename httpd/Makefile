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
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

asan: ${PROG_ASAN}

build: ${PROG}

cov: ${PROG_COV}

debug: ${PROG_DEBUG}

fx: kc3_httpd
	${MAKE} -C fx

fx_asan: ${PROG_ASAN}
	${MAKE} -C fx asan

fx_cov: ${PROG_COV}
	${MAKE} -C fx cov

fx_debug: ${PROG_DEBUG}
	${MAKE} -C fx debug

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3_httpd.html

gdb_fx: ${PROG_DEBUG}
	cd fx && gdb ../${PROG_DEBUG}

gdb_fx_asan: ${PROG_ASAN}
	cd fx && gdb ../${PROG_ASAN}

gdb_httpd: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${DESTDIR}${prefix}/bin
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 kc3_httpd ${DESTDIR}${prefix}/bin/kc3_httpd

lldb_httpd: debug
	if [ -f ${PROG_DEBUG}.core ]; then lldb ${PROG_DEBUG} ${PROG_DEBUG}.core; else lldb ${PROG_DEBUG}; fi

uninstall:
	rm -f ${prefix}/bin/kc3_httpd

.PHONY: \
	all \
	asan \
	cov \
	debug \
	fx \
	fx_asan \
	fx_cov \
	fx_debug \
	gdb_fx \
	gdb_fx_asan \
	gdb_httpd \
	gdb_httpd_asan \
	install \
	lldb_fx \
	lldb_fx_asan \
	lldb_httpd \
	lldb_httpd_asan \
	update_sources \
	uninstall
