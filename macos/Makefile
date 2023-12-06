## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
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

DMG_FILES = \
	../libc3/window/cairo/quartz/demo/c3_window_cairo_quartz_demo.app \
	../libc3/window/cairo/quartz/demo/c3_window_cairo_quartz_demo_debug.app \
	../libc3/window/sdl2/demo/macos/c3_window_sdl2_demo.app \
	../libc3/window/sdl2/demo/macos/c3_window_sdl2_demo_debug.app \

DESTDIR = c3-v${C3_VERSION}
DMG = ${DESTDIR}.dmg

CLEANFILES = ${DMG} ${DESTDIR} ${DESTDIR}.old

clean:
	rm -rf ${CLEANFILES}

dmg:
	rm -rf ${DESTDIR}.old
	if [ -d ${DESTDIR} ]; then mv ${DESTDIR} ${DESTDIR}.old; fi
	mkdir ${DESTDIR}
	cp -r ${DMG_FILES} ${DESTDIR}
	hdiutil create -volname "C3 v${C3_VERSION}" -srcfolder ${DESTDIR} -ov -format UDZO ${DESTDIR}.dmg

.PHONY: all clean dmg

include ../config.mk
