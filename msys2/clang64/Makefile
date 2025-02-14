
DEST = kc3-${KC3_VERSION}.win64.tmp

release: kc3-${KC3_VERSION}.win64.zip

kc3-${KC3_VERSION}.win64.zip: kc3-${KC3_VERSION}.win64
	rm -f kc3-${KC3_VERSION}.win64.zip
	zip -r kc3-${KC3_VERSION}.win64.zip kc3-${KC3_VERSION}.win64

kc3-${KC3_VERSION}.win64:
	rm -rf ${DEST}
	mkdir -p ${DEST}/test/ikc3
	cp -a "${SRC_TOP}/README.md" ${DEST}
	cp -a /clang64/bin/edit.dll ${DEST}
#	cp -a /usr/bin/msys-2.0.dll ${DEST}
#	cp -a /usr/bin/msys-2.0.dll ${DEST}/test
#	cp -a /usr/bin/msys-gcc_s-seh-1.dll ${DEST}
#	cp -a /usr/bin/msys-gcc_s-seh-1.dll ${DEST}/test
	cp -a /clang64/bin/libbsd-0.dll ${DEST}
	cp -a /clang64/bin/libbsd-0.dll ${DEST}/test
	cp -a /clang64/bin/libdl.dll ${DEST}
	cp -a /clang64/bin/libdl.dll ${DEST}/test
	cp -a /clang64/bin/libmd-0.dll ${DEST}
	cp -a /clang64/bin/libmd-0.dll ${DEST}/test
	cp -a "${SRC_TOP}"/libkc3/.libs/msys-kc3*-0.dll ${DEST}
	cp -a "${SRC_TOP}"/libkc3/.libs/msys-kc3*-0.dll ${DEST}/test
	cp -a "${SRC_TOP}"/window/.libs/msys-kc3_window*-0.dll ${DEST}
	cp -a "${SRC_TOP}"/window/cairo/.libs/msys-kc3_window_cairo*-0.dll ${DEST}
#	cp -a /clang64/bin/libgcc_s_seh-1.dll ${DEST}
	cp -a /clang64/bin/libcairo-2.dll ${DEST}
#	cp -a "${SRC_TOP}/window/cairo/win32/.libs/libkc3_window_cairo_win32*-0.dll" ${DEST}
	cp -a "${SRC_TOP}/window/cairo/win32/demo/.libs/kc3_window_cairo_win32_demo.exe" ${DEST}
	cp -a /clang64/bin/libxkbcommon-0.dll ${DEST}
	cp -a /clang64/bin/libwinpthread-1.dll ${DEST}
#	cp -a /clang64/bin/libstdc++-6.dll ${DEST}
	cp -a /clang64/bin/libfontconfig-1.dll ${DEST}
	cp -a /clang64/bin/libfreetype-6.dll ${DEST}
	cp -a /clang64/bin/libpixman-1-0.dll ${DEST}
	cp -a /clang64/bin/zlib1.dll ${DEST}
	cp -a /clang64/bin/libpng16-16.dll ${DEST}
	cp -a /clang64/bin/libexpat-1.dll ${DEST}
	cp -a /clang64/bin/libiconv-2.dll ${DEST}
	cp -a /clang64/bin/libintl-8.dll ${DEST}
	cp -a /clang64/bin/libbz2-1.dll ${DEST}
	cp -a /clang64/bin/libbrotlidec.dll ${DEST}
	cp -a /clang64/bin/libharfbuzz-0.dll ${DEST}
	cp -a /clang64/bin/libbrotlicommon.dll ${DEST}
	cp -a /clang64/bin/libglib-2.0-0.dll ${DEST}
	cp -a /clang64/bin/libgraphite2.dll ${DEST}
	cp -a /clang64/bin/libpcre2-8-0.dll ${DEST}
	cp -a "${SRC_TOP}"/lib ${DEST}/
	cp -a "${SRC_TOP}"/kc3s/.libs/kc3s*.exe ${DEST}
	cp -a "${SRC_TOP}"/ikc3/.libs/ikc3*.exe ${DEST}
	cp -a "${SRC_TOP}"/test/.libs/libkc3*.exe ${DEST}/test
	cp -a "${SRC_TOP}"/test/ikc3_test ${DEST}/test
	cp -a "${SRC_TOP}"/test/*.facts ${DEST}/test
	cp -a "${SRC_TOP}"/test/ikc3/*.kc3 "${SRC_TOP}"/test/ikc3/*.expected ${DEST}/test/ikc3
	rm -rf kc3-${KC3_VERSION}.win64.old
	-mv kc3-${KC3_VERSION}.win64 kc3-${KC3_VERSION}.win64.old
	mv ${DEST} kc3-${KC3_VERSION}.win64

.PHONY: kc3-${KC3_VERSION}.win64 release

include config.mk
