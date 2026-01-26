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
	${SRC_TOP}/img/flaps.png \
	${SRC_TOP}/img/fly-dead.png \
	${SRC_TOP}/img/fly-noto.png \
	${SRC_TOP}/img/toast.png

FONT_SOURCES = \
	${SRC_TOP}/fonts/Computer\ Modern \
	${SRC_TOP}/fonts/Courier\ New \

RESOURCES = \
	${SRC_TOP}/img/kc3.icns

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

include config.mk
include sources.mk

build:
	${MAKE} ${APP_PROG}
	${MAKE} ${APP}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP}/Contents/img/
	rsync -aP ${RESOURCES} ${APP}/Contents/Resources/
	cp ${SRC_DIR}/app_info.plist ${APP}/Contents/Info.plist
	rsync -aP --delete ${SRC_TOP}/lib ${APP}/Contents/
	touch ${APP}

asan:
	${MAKE} ${APP_PROG_ASAN}
	${MAKE} ${APP_ASAN}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP_ASAN}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP_ASAN}/Contents/img/
	rsync -aP ${RESOURCES} ${APP_ASAN}/Contents/Resources/
	cp ${SRC_DIR}/app_asan_info.plist ${APP_ASAN}/Contents/Info.plist
	rsync -aP --delete ${SRC_TOP}/lib ${APP_ASAN}/Contents/
	touch ${APP_ASAN}

cov:
	${MAKE} ${APP_PROG_COV}
	${MAKE} ${APP_COV}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP_COV}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP_COV}/Contents/img/
	rsync -aP ${RESOURCES} ${APP_COV}/Contents/Resources/
	cp ${SRC_DIR}/app_cov_info.plist ${APP_COV}/Contents/Info.plist
	rsync -aP --delete ${SRC_TOP}/lib ${APP_COV}/Contents/
	touch ${APP_COV}

debug:
	${MAKE} ${APP_PROG_DEBUG}
	${MAKE} ${APP_DEBUG}/Contents/Frameworks
	rsync -aP ${FONT_SOURCES} ${APP_DEBUG}/Contents/fonts/
	rsync -aP ${IMG_SOURCES} ${APP_DEBUG}/Contents/img/
	rsync -aP ${RESOURCES} ${APP_DEBUG}/Contents/Resources/
	cp ${SRC_DIR}/app_debug_info.plist ${APP_DEBUG}/Contents/Info.plist
	rsync -aP --delete ${SRC_TOP}/lib ${APP_DEBUG}/Contents/
	touch ${APP_DEBUG}

demo: build
	time ${APP_PROG}

demo_debug: debug
	time ${APP_PROG_DEBUG}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details ${PROG}.html

gdb_demo: debug
	if [ -f ${PROG_DEBUG}.core ]; then gdb ${PROG_DEBUG} \
	    ${PROG_DEBUG}.core; else gdb ${PROG_DEBUG}; fi

lldb_demo: debug
	lldb ${APP_PROG_DEBUG}

install:
	install -m 0755 -d ${DESTDIR}/Applications
	install -m 0755 -d ${DESTDIR}/Applications/kc3
	cp -R ${APP} ${DESTDIR}/Applications/kc3/
	touch ${DESTDIR}/Applications/kc3/${APP}
	cp -R ${APP_DEBUG} ${DESTDIR}/Applications/kc3/
	touch ${DESTDIR}/Applications/kc3/${APP_DEBUG}

uninstall:
	rm -rf /Applications/kc3/${APP}
	rm -rf /Applications/kc3/${APP_DEBUG}

.PHONY: \
	all \
	asan \
	cov \
	debug \
	demo \
	gdb_demo \
	install \
	lldb_demo \
	uninstall
