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
	${MAKE} debug
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

asan: mount_kc3fs_asan

build: mount_kc3fs

cov:   mount_kc3fs_cov

debug: mount_kc3fs_debug

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details kc3fs.html

gdb_kc3fs: debug
	if [ -f kc3fs_debug.core ]; then gdb kc3fs_debug kc3fs_debug.core; else gdb kc3fs_debug; fi

gdb_kc3fs_asan: asan
	if [ -f kc3fs_debug.core ]; then gdb kc3fs_asan kc3fs_asan.core; else gdb kc3fs_asan; fi

install:
	mkdir -p ${prefix}/bin
	install -m 755 mount_kc3fs ${prefix}/bin/mount_kc3fs

lldb_kc3fs: debug
	if [ -f kc3fs_debug.core ]; then lldb kc3fs_debug kc3fs_debug.core; else lldb kc3fs_debug; fi

.PHONY: \
	all \
	asan \
	cov \
	debug \
	gdb_kc3fs \
	install \
	lldb_kc3fs \
	update_sources \
