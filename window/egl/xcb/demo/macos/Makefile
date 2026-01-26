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

CLEANFILES += \
	${APP_PROG_DEBUG} \
	${PROG} \
	${PROG_ASAN} \
	${PROG_COV} \
	${PROG_DEBUG}

IMG_SOURCES = \
	${SRC_TOP}/img/earth.png \
	${SRC_TOP}/img/flaps.256.png \
	${SRC_TOP}/img/flaps.png \
	${SRC_TOP}/img/fly-dead.png \
	${SRC_TOP}/img/fly-noto.png \
	${SRC_TOP}/img/matrix_shade.png \
	${SRC_TOP}/img/toast.128.png \
	${SRC_TOP}/img/toast.png \

FONT_SOURCES = \
	${SRC_TOP}/fonts/Courier\ New \
	${SRC_TOP}/fonts/Noto\ Sans \

all:
	${MAKE} build
	${MAKE} debug
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk

asan:
	${MAKE} ${APP_PROG_ASAN}
	${MAKE} ${APP_ASAN}/Contents/Frameworks
	rm -rf ${APP_ASAN}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP_ASAN}/Contents/
	mkdir -p ${APP_ASAN}/Contents/img
	cp -RL ${IMG_SOURCES} ${APP_ASAN}/Contents/img/
	mkdir -p ${APP_ASAN}/Contents/fonts
	cp -RL ${FONT_SOURCES} ${APP_ASAN}/Contents/fonts/

build:
	${MAKE} ${APP_PROG}
	${MAKE} ${APP}/Contents/Frameworks
	rm -rf ${APP}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP}/Contents/
	mkdir -p ${APP}/Contents/img
	cp -LR ${IMG_SOURCES} ${APP}/Contents/img/
	mkdir -p ${APP}/Contents/fonts
	cp -LR ${FONT_SOURCES} ${APP}/Contents/fonts/

cov:
	${MAKE} ${APP_PROG_COV}
	${MAKE} ${APP_COV}/Contents/Frameworks
	rm -rf ${APP}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP_COV}/Contents/
	mkdir -p ${APP_COV}/Contents/img
	cp -RL ${IMG_SOURCES} ${APP_COV}/Contents/img/
	mkdir -p ${APP_COV}/Contents/fonts
	cp -RL ${FONT_SOURCES} ${APP_COV}/Contents/fonts/

debug:
	${MAKE} ${APP_PROG_DEBUG}
	${MAKE} ${APP_DEBUG}/Contents/Frameworks
	rm -rf ${APP}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP_DEBUG}/Contents/
	mkdir -p ${APP_DEBUG}/Contents/img
	cp -RL ${IMG_SOURCES} ${APP_DEBUG}/Contents/img/
	mkdir -p ${APP_DEBUG}/Contents/fonts
	cp -RL ${FONT_SOURCES} ${APP_DEBUG}/Contents/fonts/

demo: build
	time ${APP_PROG}.sh

demo_debug: debug
	time ${APP_PROG_DEBUG}.sh

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

lldb_demo: debug
	lldb ${APP_PROG_DEBUG}

install:
	${INSTALL} -m 755 -d ${DESTDIR}${prefix}/bin
	${INSTALL} -m 755 ${PROG} ${DESTDIR}${prefix}/bin/${PROG}

.PHONY: \
	all \
	asan \
	cov \
	debug \
	demo \
	gdb_demo \
	install
