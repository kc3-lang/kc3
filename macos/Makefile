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

all: dmg

APP_FILES = \
	../window/cairo/quartz/demo/kc3_window_cairo_quartz_demo.app \
	../window/cairo/quartz/demo/kc3_window_cairo_quartz_demo_debug.app \
	../window/sdl2/demo/macos/kc3_window_sdl2_demo.app \
	../window/sdl2/demo/macos/kc3_window_sdl2_demo_debug.app

BIN_FILES = \
	../ikc3/.libs/ikc3 \
	../ikc3/.libs/ikc3_debug \
	../kc3s/.libs/kc3s \
	../kc3s/.libs/kc3s_debug

LIB_FILES = \
	../*/.libs/libkc3*.0.dylib \
	/opt/homebrew/lib/libevent-2.1.7.dylib \
	/opt/homebrew/opt/libffi/lib/libffi.8.dylib
#	/usr/local/lib/libevent-2.1.7.dylib

CLEANFILES = ${DMG} ${DESTDIR} ${DESTDIR}.old

clean:
	rm -rf ${CLEANFILES}

dmg:
	rm -rf ${DESTDIR}.old
	if [ -d ${DESTDIR} ]; then mv ${DESTDIR} ${DESTDIR}.old; fi
	mkdir ${DESTDIR}
	cp -R ${APP_FILES} ${DESTDIR}/
	mkdir ${DESTDIR}/bin
	cp ${BIN_FILES} ${DESTDIR}/bin/
	mkdir ${DESTDIR}/lib
	cp -RL ${LIB_FILES} ${DESTDIR}/lib/
	cp -R ../lib/kc3 ${DESTDIR}/lib/
	./update_libs ${DESTDIR}
	hdiutil create -volname "KC3 v${KC3_VERSION}" -srcfolder ${DESTDIR} -ov -format UDZO ${DESTDIR}.dmg

.PHONY: all clean dmg

include config.mk
