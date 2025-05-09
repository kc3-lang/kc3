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

CLEANFILES = *.a mount_kc3fs mount_kc3fs_asan mount_kc3fs_cov mount_kc3fs_debug *.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

build: mount_kc3fs

asan: mount_kc3fs_asan

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:   mount_kc3fs_cov

debug: mount_kc3fs_debug

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3fs.html

gdb_kc3fs: debug
	if [ -f kc3fs_debug.core ]; then gdb .libs/kc3fs_debug kc3fs_debug.core; else gdb .libs/kc3fs_debug; fi

gdb_kc3fs_asan: asan
	if [ -f kc3fs_debug.core ]; then gdb .libs/kc3fs_asan kc3fs_asan.core; else gdb .libs/kc3fs_asan; fi

install:
	mkdir -p ${prefix}/bin
	install -m 755 .libs/mount_kc3fs ${prefix}/bin/mount_kc3fs

lldb_kc3fs: debug
	if [ -f kc3fs_debug.core ]; then lldb .libs/kc3fs_debug kc3fs_debug.core; else lldb .libs/kc3fs_debug; fi

.PHONY: \
	all \
	asan \
	clean \
	clean_cov \
	cov \
	debug \
	distclean \
	gdb_kc3fs \
	install \
	lldb_kc3fs \
	update_sources \

include config.mk
include sources.mk
