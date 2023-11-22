
DEST = c3-${C3_VERSION}.win64.tmp

release: c3-${C3_VERSION}.win64.zip

c3-${C3_VERSION}.win64.zip: c3-${C3_VERSION}.win64
	rm c3-${C3_VERSION}.win64.zip
	zip -r c3-${C3_VERSION}.win64.zip c3-${C3_VERSION}.win64

c3-${C3_VERSION}.win64:
	rm -rf ${DEST}
	mkdir -p ${DEST}/test/ic3
	cp -a ../README.md ${DEST}
	cp -a /ucrt64/bin/libbsd-0.dll ${DEST}
	cp -a /ucrt64/bin/libbsd-0.dll ${DEST}/test
	cp -a /ucrt64/bin/libmd-0.dll ${DEST}
	cp -a /ucrt64/bin/libmd-0.dll ${DEST}/test
	cp -a ../libffi/.libs/libffi*.dll ${DEST}
	cp -a ../libffi/.libs/libffi*.dll ${DEST}/test
	cp -a ../libc3/.libs/libc3*.dll ${DEST}
	cp -a ../libc3/.libs/libc3*.dll ${DEST}/test
	cp -a ../libc3/window/.libs/libc3_window*.dll ${DEST}
	cp -a ../libc3/window/.libs/libc3_window*.dll ${DEST}/test
	cp -a ../libc3/window/cairo/.libs/libc3_window_cairo*.dll ${DEST}
	cp -a ../libc3/window/cairo/.libs/libc3_window_cairo*.dll ${DEST}/test
	cp -a ../libc3/window/cairo/win32/.libs/libc3_window_cairo_win32*.dll ${DEST}
	cp -a ../libc3/window/cairo/win32/.libs/libc3_window_cairo_win32*.dll ${DEST}/test
	cp -a ../libc3/window/cairo/win32/demo/.libs/c3_window_cairo_win32_demo.exe ${DEST}
	cp -a ../libc3/window/cairo/win32/demo/.libs/c3_window_cairo_win32_demo.exe ${DEST}/test
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
