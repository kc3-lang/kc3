
DEST = c3-${C3_VERSION}.win64.tmp

release: c3-${C3_VERSION}.win64.zip

c3-${C3_VERSION}.win64.zip: c3-${C3_VERSION}.win64
	rm c3-${C3_VERSION}.win64.zip
	zip -r c3-${C3_VERSION}.win64.zip c3-${C3_VERSION}.win64

c3-${C3_VERSION}.win64:
	rm -rf ${DEST}
	mkdir -p ${DEST}/test/ic3
	cp -a ../README.md ${DEST}
	cp -a /bin/msys-2.0.dll ${DEST}
	cp -a /bin/msys-2.0.dll ${DEST}/test
	cp -a /bin/msys-bsd-0.dll ${DEST}
	cp -a /bin/msys-bsd-0.dll ${DEST}/test
	cp -a /bin/msys-gcc_s-seh-1.dll ${DEST}
	cp -a /bin/msys-gcc_s-seh-1.dll ${DEST}/test
	cp -a /bin/msys-md-0.dll ${DEST}
	cp -a /bin/msys-md-0.dll ${DEST}/test
	cp -a ../libffi/.libs/libffi*.dll ${DEST}
	cp -a ../libffi/.libs/libffi*.dll ${DEST}/test
	cp -a ../libc3/.libs/msys-c3*.dll ${DEST}
	cp -a ../libc3/.libs/msys-c3*.dll ${DEST}/test
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
