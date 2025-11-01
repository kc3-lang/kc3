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

DEST = kc3-${C3_VERSION}.win32.tmp

IMG_SOURCES = \
	../img/earth.png \
	../img/flaps.256.png \
	../img/flaps.png \
	../img/fly-dead.png \
	../img/fly-noto.png \
	../img/toast.128.png \
	../img/toast.png \

FONT_SOURCES = \
	../fonts/Computer-Modern \
	../fonts/Courier\ New \

release: c3-${C3_VERSION}.win32.zip

c3-${C3_VERSION}.win32.zip: c3-${C3_VERSION}.win32
	rm -f c3-${C3_VERSION}.win32.zip
	zip -r c3-${C3_VERSION}.win32.zip c3-${C3_VERSION}.win32

c3-${C3_VERSION}.win32:
	rm -rf ${DEST}
	mkdir -p ${DEST}/test/ic3
	cp -a ../README.md ${DEST}
	cp -a ../c3s/c3s*.exe ${DEST}
	cp -a ../ic3/ic3*.exe ${DEST}
	rsync -aP ${IMG_SOURCES} ${DEST}/img/
	rsync -aP ${FONT_SOURCES} ${DEST}/fonts/
	cp -a ../lib ${DEST}/
	cp -a ../libkc3/libkc3*-0.dll ${DEST}
	cp -a ../libkc3/libkc3*-0.dll ${DEST}/test
	cp -a ../libkc3/window/libkc3_window*-0.dll ${DEST}
	cp -a ../libkc3/window/cairo/libkc3_window_cairo*-0.dll ${DEST}
	cp -a ../libkc3/window/cairo/demo/libkc3_window_cairo_demo*-0.dll ${DEST}
	cp -a ../libkc3/window/cairo/win32/libkc3_window_cairo_win32*-0.dll ${DEST}
	cp -a ../libkc3/window/cairo/win32/demo/c3_window_cairo_win32_demo*.exe ${DEST}
	cp -a ../libkc3/window/sdl2/demo/c3_window_sdl2_demo*.exe ${DEST}
	cp -a ../libffi/libffi-8.dll ${DEST}
	cp -a ../libffi/libffi-8.dll ${DEST}/test
	cp -a ../test/*.facts ${DEST}/test
	cp -a ../test/libkc3*.exe ${DEST}/test
	cp -a ../test/ic3/*.in ../test/ic3/*.expected ${DEST}/test/ic3
	cp -a ../test/ic3_test ${DEST}/test
	cp -a /mingw32/bin/edit.dll ${DEST}
	cp -a /mingw32/bin/edit.dll ${DEST}/test
	cp -a /mingw32/bin/libbrotlicommon.dll ${DEST}
	cp -a /mingw32/bin/libbrotlidec.dll ${DEST}
	cp -a /mingw32/bin/libbsd-0.dll ${DEST}
	cp -a /mingw32/bin/libbsd-0.dll ${DEST}/test
	cp -a /mingw32/bin/libbz2-1.dll ${DEST}
	cp -a /mingw32/bin/libcairo-2.dll ${DEST}
	cp -a /mingw32/bin/libdl.dll ${DEST}
	cp -a /mingw32/bin/libdl.dll ${DEST}/test
	cp -a /mingw32/bin/libexpat-1.dll ${DEST}
	cp -a /mingw32/bin/libfontconfig-1.dll ${DEST}
	cp -a /mingw32/bin/libfreetype-6.dll ${DEST}
	cp -a /mingw32/bin/libftgl-2.dll ${DEST}
	cp -a /mingw32/bin/SDL2.dll ${DEST}
	cp -a /mingw32/bin/libgcc_s_dw2-1.dll ${DEST}
	cp -a /mingw32/bin/libgcc_s_dw2-1.dll ${DEST}/test
	cp -a /mingw32/bin/libglib-2.0-0.dll ${DEST}
	cp -a /mingw32/bin/libgraphite2.dll ${DEST}
	cp -a /mingw32/bin/libharfbuzz-0.dll ${DEST}
	cp -a /mingw32/bin/libiconv-2.dll ${DEST}
	cp -a /mingw32/bin/libintl-8.dll ${DEST}
	cp -a /mingw32/bin/libmd-0.dll ${DEST}
	cp -a /mingw32/bin/libmd-0.dll ${DEST}/test
	cp -a /mingw32/bin/libpcre2-8-0.dll ${DEST}
	cp -a /mingw32/bin/libpixman-1-0.dll ${DEST}
	cp -a /mingw32/bin/libpng16-16.dll ${DEST}
	cp -a /mingw32/bin/libstdc++-6.dll ${DEST}
	cp -a /mingw32/bin/libwinpthread-1.dll ${DEST}
	cp -a /mingw32/bin/libxkbcommon-0.dll ${DEST}
	cp -a /mingw32/bin/zlib1.dll ${DEST}
	rm -rf c3-${C3_VERSION}.win32.old
	if [ -d c3-${C3_VERSION}.win32 ]; then mv c3-${C3_VERSION}.win32 c3-${C3_VERSION}.win32.old; fi
	mv ${DEST} c3-${C3_VERSION}.win32

.PHONY: c3-${C3_VERSION}.win32 release

include config.mk
