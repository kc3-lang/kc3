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

CLEANFILES = *.a ${APP_PROG} ${APP_PROG_ASAN} ${APP_PROG_COV} \
	${APP_PROG_DEBUG} ${PROG} ${PROG_ASAN} ${PROG_COV} ${PROG_DEBUG} \
	*.css *.gcno *.html *.o .libs *.lo

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

IMG_SOURCES = \
	../../../../img/earth.png \
	../../../../img/flaps.256.png \
	../../../../img/flaps.png \
	../../../../img/fly-dead.png \
	../../../../img/fly-noto.png \
	../../../../img/matrix_shade.png \
	../../../../img/toast.128.png \
	../../../../img/toast.png \

FONT_SOURCES = \
	../../../../fonts/Courier\ New \
	../../../../fonts/Noto\ Sans \

build:
	${MAKE} ${APP_PROG}
	${MAKE} ${APP}/Contents/Frameworks
	rsync -aP --delete ../../../../lib ${APP}/Contents/
	rsync -aP ${IMG_SOURCES} ${APP}/Contents/img/
	rsync -aP ${FONT_SOURCES} ${APP}/Contents/fonts/

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan:
	${MAKE} ${APP_PROG_ASAN}
	${MAKE} ${APP_ASAN}/Contents/Frameworks
	rsync -aP --delete ../../../../lib ${APP_ASAN}/Contents/
	rsync -aP ${IMG_SOURCES} ${APP_ASAN}/Contents/img/
	rsync -aP ${FONT_SOURCES} ${APP_ASAN}/Contents/fonts/

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${APP_PROG_COV}
	${MAKE} ${APP_COV}/Contents/Frameworks
	rsync -aP --delete ../../../../lib ${APP_COV}/Contents/
	rsync -aP ${IMG_SOURCES} ${APP_COV}/Contents/img/
	rsync -aP ${FONT_SOURCES} ${APP_COV}/Contents/fonts/

debug:
	${MAKE} ${APP_PROG_DEBUG}
	${MAKE} ${APP_DEBUG}/Contents/Frameworks
	rsync -aP --delete ../../../../lib ${APP_DEBUG}/Contents/
	rsync -aP ${IMG_SOURCES} ${APP_DEBUG}/Contents/img/
	rsync -aP ${FONT_SOURCES} ${APP_DEBUG}/Contents/fonts/

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
	install -m 755 -d ${prefix}/bin
	install -m 755 ${PROG} ${prefix}/bin/${PROG}

.PHONY: \
	all \
	asan \
	clean \
	clean_cov \
	cov \
	debug \
	demo \
	distclean \
	gdb_demo

include config.mk
