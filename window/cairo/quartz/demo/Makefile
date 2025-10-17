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

CLEANFILES = *.a ${APP_PROG} ${APP_PROG_ASAN} ${APP_PROG_COV} \
	${APP_PROG_DEBUG} ${PROG} ${PROG_ASAN} ${PROG_COV} ${PROG_DEBUG} \
	*.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

IMG_SOURCES = \
	${SRC_TOP}/img/flaps.png \
	${SRC_TOP}/img/fly-dead.png \
	${SRC_TOP}/img/fly-noto.png \
	${SRC_TOP}/img/toast.png

FONT_SOURCES = \
	${SRC_TOP}/fonts/Computer\ Modern \
	${SRC_TOP}/fonts/Courier\ New \

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

build:
	${MAKE} ${APP_PROG}
	${MAKE} ${APP}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP}/Contents/img/
	rsync -aP --delete ${SRC_TOP}/lib ${APP}/Contents/

asan:
	${MAKE} ${APP_PROG_ASAN}
	${MAKE} ${APP_ASAN}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP_ASAN}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP_ASAN}/Contents/img/
	rsync -aP --delete ${SRC_TOP}/lib ${APP_ASAN}/Contents/

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${APP_PROG_COV}
	${MAKE} ${APP_COV}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP_COV}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP_COV}/Contents/img/
	rsync -aP --delete ${SRC_TOP}/lib ${APP_COV}/Contents/

debug:
	${MAKE} ${APP_PROG_DEBUG}
	${MAKE} ${APP_DEBUG}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP_DEBUG}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP_DEBUG}/Contents/img/
	rsync -aP --delete ${SRC_TOP}/lib ${APP_DEBUG}/Contents/

demo: build
	time ${APP_PROG}

demo_debug: debug
	time ${APP_PROG_DEBUG}

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb .libs/${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb .libs/${PROG_DEBUG}; fi

lldb_demo: debug
	lldb ${APP_PROG_DEBUG}

install:

.PHONY: \
	all \
	asan \
	clean \
	clean_cov \
	cov \
	debug \
	demo \
	distclean \
	gdb_demo \
	lldb_demo

include config.mk
include sources.mk
