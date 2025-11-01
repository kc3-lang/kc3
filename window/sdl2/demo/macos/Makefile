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

CLEANFILES_COV = *.css *.gcda *.html
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

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

RESOURCES = \
	${SRC_TOP}/img/kc3.icns

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

build:
	${MAKE} ${APP_PROG}
	cp ${SRC_DIR}/app_info.plist ${APP}/Contents/Info.plist
	cp ${SRC_DIR}/kc3_window_sdl2_demo.sh ${APP_PROG}.sh
	${MAKE} ${APP}/Contents/Frameworks
	rm -rf ${APP}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP}/Contents/
	mkdir -p ${APP}/Contents/img/
	cp -LR ${IMG_SOURCES} ${APP}/Contents/img/
	mkdir -p ${APP}/Contents/fonts/
	cp -LR ${FONT_SOURCES} ${APP}/Contents/fonts/
	mkdir -p ${APP}/Contents/Resources/
	cp -RL ${RESOURCES} ${APP}/Contents/Resources/
	touch ${APP}

asan:
	${MAKE} ${APP_PROG_ASAN}
	cp ${SRC_DIR}/app_asan_info.plist ${APP_ASAN}/Contents/Info.plist
	cp ${SRC_DIR}/kc3_window_sdl2_demo_asan.sh ${APP_PROG_ASAN}.sh
	${MAKE} ${APP_ASAN}/Contents/Frameworks
	rm -rf ${APP_ASAN}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP_ASAN}/Contents/
	mkdir -p ${APP_ASAN}/Contents/fonts/
	cp -RL ${FONT_SOURCES} ${APP_ASAN}/Contents/fonts/
	mkdir -p ${APP_ASAN}/Contents/img/
	cp -RL ${IMG_SOURCES} ${APP_ASAN}/Contents/img/
	mkdir -p ${APP_ASAN}/Contents/Resources/
	cp -RL ${RESOURCES} ${APP_ASAN}/Contents/Resources/
	touch ${APP_ASAN}

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov:
	${MAKE} ${APP_PROG_COV}
	cp ${SRC_DIR}/app_cov_info.plist ${APP_COV}/Contents/Info.plist
	cp ${SRC_DIR}/kc3_window_sdl2_demo_cov.sh ${APP_PROG_COV}.sh
	${MAKE} ${APP_COV}/Contents/Frameworks
	rm -rf ${APP_COV}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP_COV}/Contents/
	mkdir -p ${APP_COV}/Contents/fonts/
	cp -RL ${FONT_SOURCES} ${APP_COV}/Contents/fonts/
	mkdir -p ${APP_COV}/Contents/img/
	cp -RL ${IMG_SOURCES} ${APP_COV}/Contents/img/
	mkdir -p ${APP_COV}/Contents/Resources/
	cp -RL ${RESOURCES} ${APP_COV}/Contents/Resources/
	touch ${APP_COV}

debug:
	${MAKE} ${APP_PROG_DEBUG}
	cp ${SRC_DIR}/app_debug_info.plist ${APP_DEBUG}/Contents/Info.plist
	cp ${SRC_DIR}/kc3_window_sdl2_demo_debug.sh ${APP_PROG_DEBUG}.sh
	${MAKE} ${APP_DEBUG}/Contents/Frameworks
	rm -rf ${APP_DEBUG}/Contents/lib
	cp -R ${SRC_TOP}/lib ${APP_DEBUG}/Contents/
	mkdir -p ${APP_DEBUG}/Contents/fonts/
	cp -RL ${FONT_SOURCES} ${APP_DEBUG}/Contents/fonts/
	mkdir -p ${APP_DEBUG}/Contents/img/
	cp -RL ${IMG_SOURCES} ${APP_DEBUG}/Contents/img/
	mkdir -p ${APP_DEBUG}/Contents/Resources/
	cp -RL ${RESOURCES} ${APP_DEBUG}/Contents/Resources/
	touch ${APP_DEBUG}

demo: build
	time ${APP_PROG}.sh

demo_debug: debug
	time ${APP_PROG_DEBUG}.sh

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

lldb_demo: debug
	lldb ${APP_PROG_DEBUG}

install:
	install -m 0755 -d ${DESTDIR}${prefix}
	cp -R ${APP} ${DESTDIR}${prefix}/
	touch ${DESTDIR}${prefix}/${APP}
	cp -R ${APP_DEBUG} ${DESTDIR}${prefix}/
	touch ${DESTDIR}${prefix}/${APP_DEBUG}

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
