
DEST = kc3-${KC3_VERSION}.win64
DEST_OLD = ${DEST}.old
DEST_TMP = ${DEST}.tmp
DEST_ZIP = ${DEST}.zip

release: ${DEST_ZIP}

clean:
	rm -rf ${DEST} ${DEST_TMP} ${DEST_ZIP}

${DEST}: ${DEST_TMP}
	rm -rf ${DEST_OLD}
	if test -d ${DEST}; then mv ${DEST} ${DEST_OLD}; fi
	mv ${DEST_TMP} ${DEST}

${DEST_TMP}:
	rm -rf ${DEST_TMP}
	mkdir -p ${DEST_TMP}/test/ikc3
	cp -a "${SRC_TOP}/README.md" ${DEST_TMP}
	cp -a /clang64/bin/edit.dll ${DEST_TMP}
	cp -a /usr/bin/msys-2.0.dll ${DEST_TMP}
#	cp -a /usr/bin/msys-gcc_s-seh-1.dll ${DEST_TMP}
#	cp -a /usr/bin/msys-gcc_s-seh-1.dll ${DEST_TMP}/test
	cp -a /clang64/bin/libbsd-0.dll ${DEST_TMP}
	cp -a /clang64/bin/libbsd-0.dll ${DEST_TMP}/test
	cp -a /clang64/bin/libdl.dll ${DEST_TMP}
	cp -a /clang64/bin/libdl.dll ${DEST_TMP}/test
	cp -a /clang64/bin/libmd-0.dll ${DEST_TMP}
	cp -a /clang64/bin/libmd-0.dll ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/libkc3/libkc3*-0.dll ${DEST_TMP}
	cp -a "${SRC_TOP}"/libkc3/libkc3*-0.dll ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/window/libkc3_window*-0.dll ${DEST_TMP}
	cp -a "${SRC_TOP}"/window/cairo/libkc3_window_cairo*-0.dll ${DEST_TMP}
	cp -a "${SRC_TOP}"/window/cairo/demo/libkc3_window_cairo_demo*-0.dll ${DEST_TMP}
	cp -a "${SRC_TOP}"/window/cairo/win32/libkc3_window_cairo_win32*-0.dll ${DEST_TMP}
#	cp -a /clang64/bin/libgcc_s_seh-1.dll ${DEST_TMP}
	cp -a /clang64/bin/libcairo-2.dll ${DEST_TMP}
#	cp -a "${SRC_TOP}/window/cairo/win32/libkc3_window_cairo_win32*-0.dll" ${DEST_TMP}
	cp -a "${SRC_TOP}/window/cairo/win32/demo/kc3_window_cairo_win32_demo.exe" ${DEST_TMP}
	cp -a "${SRC_TOP}/window/cairo/win32/demo/kc3_window_cairo_win32_demo_debug.exe" ${DEST_TMP}
	cp -a "${SRC_TOP}"/window/sdl2/libkc3_window_sdl2*-0.dll ${DEST_TMP}
	cp -a "${SRC_TOP}/window/sdl2/demo/kc3_window_sdl2_demo.exe" ${DEST_TMP}
	cp -a "${SRC_TOP}/window/sdl2/demo/kc3_window_sdl2_demo_debug.exe" ${DEST_TMP}
	cp -a "${SRC_TOP}/window/sdl2/kubz/kubz.exe" ${DEST_TMP}
	cp -a "${SRC_TOP}/window/sdl2/kubz/kubz_debug.exe" ${DEST_TMP}
	cp -a /clang64/bin/SDL2.dll ${DEST_TMP}
	cp -a /clang64/bin/libxkbcommon-0.dll ${DEST_TMP}
	cp -a /clang64/bin/libwinpthread-1.dll ${DEST_TMP}
	cp -a /clang64/bin/libwinpthread-1.dll ${DEST_TMP}/test
#	cp -a /clang64/bin/libstdc++-6.dll ${DEST_TMP}
	cp -a /clang64/bin/libfontconfig-1.dll ${DEST_TMP}
	cp -a /clang64/bin/libfreetype-6.dll ${DEST_TMP}
	cp -a /clang64/bin/libpixman-1-0.dll ${DEST_TMP}
	cp -a /clang64/bin/zlib1.dll ${DEST_TMP}
	cp -a /clang64/bin/libpng16-16.dll ${DEST_TMP}
	cp -a /clang64/bin/libexpat-1.dll ${DEST_TMP}
	cp -a /clang64/bin/libiconv-2.dll ${DEST_TMP}
	cp -a /clang64/bin/libintl-8.dll ${DEST_TMP}
	cp -a /clang64/bin/libbz2-1.dll ${DEST_TMP}
	cp -a /clang64/bin/libbrotlidec.dll ${DEST_TMP}
	cp -a /clang64/bin/libharfbuzz-0.dll ${DEST_TMP}
	cp -a /clang64/bin/libbrotlicommon.dll ${DEST_TMP}
	cp -a /clang64/bin/libglib-2.0-0.dll ${DEST_TMP}
	cp -a /clang64/bin/libgraphite2.dll ${DEST_TMP}
	cp -a /clang64/bin/libpcre2-8-0.dll ${DEST_TMP}
	cp -a /clang64/bin/libc++.dll ${DEST_TMP}
	cp -a /clang64/bin/libffi-8.dll ${DEST_TMP}
	cp -a /clang64/bin/libffi-8.dll ${DEST_TMP}/test
	mkdir -p ${DEST_TMP}/fonts
	cp -RL "${SRC_TOP}"/fonts/Courier\ New ${DEST_TMP}/fonts/
	cp -RL "${SRC_TOP}"/fonts/Noto\ Sans ${DEST_TMP}/fonts/
	cp -RL "${SRC_TOP}"/lib ${DEST_TMP}/
	cp -a "${SRC_TOP}"/kc3s/kc3s*.exe ${DEST_TMP}
	cp -a "${SRC_TOP}"/ikc3/ikc3*.exe ${DEST_TMP}
	cp -a "${SRC_TOP}"/test/libkc3*.exe ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/test/ikc3_test ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/test/*.facts ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/test/*.kc3 ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/test/*.kc3m ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/test/zero ${DEST_TMP}/test
	cp -a "${SRC_TOP}"/test/ikc3/*.kc3 "${SRC_TOP}"/test/ikc3/*.expected ${DEST_TMP}/test/ikc3

${DEST_ZIP}: ${DEST}
	rm -f ${DEST_ZIP}~
	if test -f ${DEST_ZIP}; then mv ${DEST_ZIP} ${DEST_ZIP}~; fi
	zip -r ${DEST_ZIP} ${DEST}

.PHONY: clean ${DEST} ${DEST_TMP} release

include config.mk
