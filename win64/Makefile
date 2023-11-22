
DEST = c3-${C3_VERSION}.win64.tmp

release: c3-${C3_VERSION}.win64.zip

c3-${C3_VERSION}.win64.zip: c3-${C3_VERSION}.win64
	rm -f c3-${C3_VERSION}.win64.zip
	zip -r c3-${C3_VERSION}.win64.zip c3-${C3_VERSION}.win64

c3-${C3_VERSION}.win64:
	rm -rf ${DEST}
	mkdir -p ${DEST}/test/ic3
	cp -a ../README.md ${DEST}
	cp -a /usr/bin/msys-2.0.dll ${DEST}
	cp -a /usr/bin/msys-2.0.dll ${DEST}/test
	cp -a /usr/bin/msys-gcc_s-seh-1.dll ${DEST}
	cp -a /usr/bin/msys-gcc_s-seh-1.dll ${DEST}/test
	cp -a /usr/bin/msys-bsd-0.dll ${DEST}
	cp -a /usr/bin/msys-bsd-0.dll ${DEST}/test
	cp -a /usr/bin/msys-md-0.dll ${DEST}
	cp -a /usr/bin/msys-md-0.dll ${DEST}/test
	cp -a ../libffi/.libs/msys-ffi-8.dll ${DEST}
	cp -a ../libffi/.libs/msys-ffi-8.dll ${DEST}/test
	cp -a ../libc3/.libs/msys-c3*-0.dll ${DEST}
	cp -a ../libc3/.libs/msys-c3*-0.dll ${DEST}/test
	cp -a ../libc3/window/.libs/msys-c3_window*-0.dll ${DEST}
	cp -a ../libc3/window/cairo/.libs/msys-c3_window_cairo*-0.dll ${DEST}
	cp -a /mingw64/bin/libgcc_s_seh-1.dll ${DEST}
	cp -a /mingw64/bin/libcairo-2.dll ${DEST}
	cp -a ../libc3/window/cairo/win32/.libs/msys-c3_window_cairo_win32*-0.dll ${DEST}
	cp -a ../libc3/window/cairo/win32/demo/.libs/c3_window_cairo_win32_demo.exe ${DEST}
	cp -a /mingw64/bin/libxkbcommon-0.dll ${DEST}
	cp -a /mingw64/bin/libwinpthread-1.dll ${DEST}
	cp -a /mingw64/bin/libstdc++-6.dll ${DEST}
	cp -a /mingw64/bin/libfontconfig-1.dll ${DEST}
	cp -a /mingw64/bin/libfreetype-6.dll ${DEST}
	cp -a /mingw64/bin/libpixman-1-0.dll ${DEST}
	cp -a /mingw64/bin/zlib1.dll ${DEST}
	cp -a /mingw64/bin/libpng16-16.dll ${DEST}
	cp -a /mingw64/bin/libexpat-1.dll ${DEST}
	cp -a /mingw64/bin/libiconv-2.dll ${DEST}
	cp -a /mingw64/bin/libintl-8.dll ${DEST}
	cp -a /mingw64/bin/libbz2-1.dll ${DEST}
	cp -a /mingw64/bin/libbrotlidec.dll ${DEST}
	cp -a /mingw64/bin/libharfbuzz-0.dll ${DEST}
	cp -a /mingw64/bin/libbrotlicommon.dll ${DEST}
	cp -a /mingw64/bin/libglib-2.0-0.dll ${DEST}
	cp -a /mingw64/bin/libgraphite2.dll ${DEST}
	cp -a /mingw64/bin/libpcre2-8-0.dll ${DEST}
	cp -a ../lib ${DEST}/
	cp -a ../c3s/.libs/c3s*.exe ${DEST}
	cp -a ../ic3/.libs/ic3*.exe ${DEST}
	cp -a ../test/.libs/libc3*.exe ${DEST}/test
	cp -a ../test/ic3_test ${DEST}/test
	cp -a ../test/*.facts ${DEST}/test
	cp -a ../test/ic3/*.in ../test/ic3/*.expected ${DEST}/test/ic3
	rm -rf c3-${C3_VERSION}.win64.old
	mv c3-${C3_VERSION}.win64 c3-${C3_VERSION}.win64.old
	mv ${DEST} c3-${C3_VERSION}.win64

.PHONY: c3-${C3_VERSION}.win64 release

include config.mk
