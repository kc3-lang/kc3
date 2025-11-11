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

all:
	${MAKE} gen
	${MAKE} -C libtommath all
	${MAKE} -C libkc3 all
	${MAKE} -C socket all
	${MAKE} -C tls all
	${MAKE} -C ikc3 all
	${MAKE} -C kc3s all
	${MAKE} -C kpkg all
	${MAKE} -C ekc3 all
	${MAKE} -C event all
	${MAKE} -C json all
	${MAKE} -C markdown all
	${MAKE} -C pdf all
	${MAKE} -C smtp all
	${MAKE} -C http all
	${MAKE} -C httpd all
	${MAKE} -C test all
	${MAKE} -C gl all
	${MAKE} -C window all
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 all; fi

include config.mk
include sources.mk

DIST = kc3-${VER}

android:
	${MAKE} -C android/egl/demo

asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C kpkg asan
	${MAKE} -C ekc3 asan
	${MAKE} -C event asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C pdf asan
	${MAKE} -C smtp asan
	${MAKE} -C http asan
	${MAKE} -C httpd asan
	${MAKE} -C test asan
	${MAKE} -C gl asan
	${MAKE} -C window asan
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 asan; fi

assets:
	make -C test/httpd/assets

build:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C kpkg build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C pdf build
	${MAKE} -C smtp build
	${MAKE} -C http build
	${MAKE} -C httpd build
	${MAKE} -C test build
	${MAKE} -C gl build
	${MAKE} -C window build
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 build; fi

check: test

clean::
	${MAKE} -C libtommath clean
	${MAKE} -C libkc3 clean
	${MAKE} -C socket clean
	${MAKE} -C tls clean
	${MAKE} -C ikc3 clean
	${MAKE} -C kc3s clean
	${MAKE} -C kpkg clean
	${MAKE} -C ekc3 clean
	${MAKE} -C event clean
	${MAKE} -C json clean
	${MAKE} -C markdown clean
	${MAKE} -C pdf clean
	${MAKE} -C smtp clean
	${MAKE} -C http clean
	${MAKE} -C httpd clean
	${MAKE} -C test clean
	${MAKE} -C gl clean
	${MAKE} -C window clean
	${MAKE} -C gtk4 clean

clean_cov::
	${MAKE} -C libtommath clean_cov
	${MAKE} -C libkc3 clean_cov
	${MAKE} -C socket clean_cov
	${MAKE} -C tls clean_cov
	${MAKE} -C ikc3 clean_cov
	${MAKE} -C kc3s clean_cov
	${MAKE} -C kpkg clean_cov
	${MAKE} -C ekc3 clean_cov
	${MAKE} -C event clean_cov
	${MAKE} -C json clean_cov
	${MAKE} -C markdown clean_cov
	${MAKE} -C pdf clean_cov
	${MAKE} -C smtp clean_cov
	${MAKE} -C http clean_cov
	${MAKE} -C httpd clean_cov
	${MAKE} -C test clean_cov
	${MAKE} -C gl clean_cov
	${MAKE} -C window clean_cov
	${MAKE} -C gtk4 clean_cov

clean_dump:
	rm -f lib/kc3/0.1/kc3.dump
	rm -f lib/kc3/0.1/kpkg.dump
	rm -f test/httpd/kc3.dump
	rm -f httpd/fx/kc3.dump

cov:
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C kpkg cov
	${MAKE} -C ekc3 cov
	${MAKE} -C event cov
	${MAKE} -C json cov
	${MAKE} -C markdown cov
	${MAKE} -C pdf cov
	${MAKE} -C smtp cov
	${MAKE} -C http cov
	${MAKE} -C httpd cov
	${MAKE} -C test cov
	${MAKE} -C gl cov
	${MAKE} -C window cov
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 cov; fi

deb:
	${MAKE} -C release/v${VER} deb

debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C kpkg debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C pdf debug
	${MAKE} -C smtp debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test debug
	${MAKE} -C gl debug
	${MAKE} -C window debug
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 debug; fi

demo: build
	${MAKE} -C window demo

demo_asan: asan
	${MAKE} -C window demo_asan

demo_cairo: build
	${MAKE} -C window demo_cairo

demo_cairo_asan: asan
	${MAKE} -C window demo_cairo_asan

demo_cairo_cov: cov
	${MAKE} -C window demo_cairo_cov

demo_cairo_debug: debug
	${MAKE} -C window demo_cairo_debug

demo_cov: cov
	${MAKE} -C window demo_cov

demo_debug: debug
	${MAKE} -C window demo_debug

demo_drm: build
	${MAKE} -C window/egl/drm demo

demo_drm_asan: asan
	${MAKE} -C window/egl/drm demo_asan

demo_drm_cov: cov
	${MAKE} -C window/egl/drm demo_cov

demo_drm_debug: debug
	${MAKE} -C window/egl/drm demo_debug

demo_egl: build
	${MAKE} -C window demo_egl

demo_egl_asan: asan
	${MAKE} -C window demo_egl_asan

demo_egl_cov: cov
	${MAKE} -C window demo_egl_cov

demo_egl_debug: debug
	${MAKE} -C window demo_egl_debug

demo_gl: build
	${MAKE} -C window demo_gl

demo_gl_asan: asan
	${MAKE} -C window demo_gl_asan

demo_gl_cov: cov
	${MAKE} -C window demo_gl_cov

demo_gl_debug: debug
	${MAKE} -C window demo_gl_debug

demo_kubz: build
	${MAKE} -C window/sdl2/kubz demo

demo_kubz_debug: debug
	${MAKE} -C window/sdl2/kubz demo_debug

dist: kc3-${KC3_VERSION}.tar.gz

DIST_DMG = kc3-v${KC3_VERSION}-${MACHINE}

dist_dmg:
	mkdir -p out/dmg
	cd out/dmg && \
	    DESTDIR="$$PWD/install" ../../configure --prefix /Applications/kc3 && \
	    make -j8 && \
	    make install && \
	    ${SRC_TOP}/bin/update_dyld "$$PWD/install" && \
	    mkdir -p ${DIST_DMG} && \
	    cp -R install/Applications/kc3 ${DIST_DMG}/ && \
	    ln -sf /Applications ${DIST_DMG}/ && \
	    if [ -f ${SRC_TOP}/img/dmg_background.png ]; then \
	        mkdir -p ${DIST_DMG}/.background && \
	        cp ${SRC_TOP}/img/dmg_background.png ${DIST_DMG}/.background/; \
	    fi && \
	    hdiutil create -volname "KC3 v${KC3_VERSION}" \
		-srcfolder ${DIST_DMG} -ov -format UDRW ${DIST_DMG}.tmp.dmg && \
	    hdiutil attach ${DIST_DMG}.tmp.dmg -mountpoint /Volumes/KC3 && \
	    sleep 5 && \
	    osascript -e 'with timeout of 300 seconds' \
		-e 'tell application "Finder"' \
		-e 'tell disk "KC3"' \
		-e 'open' \
		-e 'delay 2' \
		-e 'set current view of container window to icon view' \
		-e 'set toolbar visible of container window to false' \
		-e 'set statusbar visible of container window to false' \
		-e 'set the bounds of container window to {100, 100, 700, 500}' \
		-e 'set viewOptions to the icon view options of container window' \
		-e 'set arrangement of viewOptions to not arranged' \
		-e 'set icon size of viewOptions to 128' \
		-e 'try' \
		-e 'set background picture of viewOptions to file ".background:dmg_background.png"' \
		-e 'end try' \
		-e 'delay 2' \
		-e 'try' \
		-e 'set position of item "kc3" to {150, 200}' \
		-e 'set position of item "Applications" to {450, 200}' \
		-e 'end try' \
		-e 'delay 2' \
		-e 'update without registering applications' \
		-e 'delay 3' \
		-e 'close' \
		-e 'end tell' \
		-e 'end tell' \
		-e 'end timeout' && \
	    sync && \
	    hdiutil detach /Volumes/KC3 && \
	    sync && \
	    sleep 15 && \
	    cp ${DIST_DMG}.tmp.dmg ${DIST_DMG}.tmp2.dmg && \
	    rm -f ${DIST_DMG}.dmg && \
	    hdiutil convert ${DIST_DMG}.tmp2.dmg -format UDZO -o ${DIST_DMG}.dmg && \
	    rm ${DIST_DMG}.tmp.dmg ${DIST_DMG}.tmp2.dmg

dist_msys2_clang64: all
	${MAKE} -C msys2/clang64

distclean::
	${MAKE} -C libtommath distclean
	${MAKE} -C libkc3 distclean
	${MAKE} -C socket distclean
	${MAKE} -C tls distclean
	${MAKE} -C ikc3 distclean
	${MAKE} -C kc3s distclean
	${MAKE} -C kpkg distclean
	${MAKE} -C ekc3 distclean
	${MAKE} -C event distclean
	${MAKE} -C json distclean
	${MAKE} -C markdown distclean
	${MAKE} -C pdf distclean
	${MAKE} -C smtp distclean
	${MAKE} -C http distclean
	${MAKE} -C httpd distclean
	${MAKE} -C test distclean
	${MAKE} -C gl distclean
	${MAKE} -C window distclean
	${MAKE} -C gtk4 distclean

dump:
	${MAKE} clean_dump
	kc3s/kc3s --trace --dump lib/kc3/0.1/kc3.dump --quit
	${MAKE} -C kpkg dump
	${MAKE} -C test/httpd dump
	${MAKE} -C httpd/fx dump

dump_debug:
	${MAKE} clean_dump
	kc3s/kc3s_debug --trace --dump lib/kc3/0.1/kc3.dump --quit
	${MAKE} -C kpkg dump_debug
	${MAKE} -C test/httpd dump_debug

dump_fx:
	${MAKE} -C httpd/fx clean_dump
	${MAKE} -C httpd/fx dump

ekc3:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build

ekc3_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan

ekc3_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov

ekc3_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug

event:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C event build

event_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C event asan

event_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C event cov

event_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C event debug

fx:
	${MAKE} -C httpd fx

fx_asan:
	${MAKE} -C httpd fx_asan

fx_cov:
	${MAKE} -C httpd fx_cov

fx_debug:
	${MAKE} -C httpd fx_debug

gcovr:
	${MAKE} -C libkc3 gcovr
	${MAKE} -C socket gcovr
	${MAKE} -C tls gcovr
	${MAKE} -C ikc3 gcovr
	${MAKE} -C kc3s gcovr
	${MAKE} -C kc3s gcovr
	${MAKE} -C ekc3 gcovr
	${MAKE} -C event gcovr
	${MAKE} -C json gcovr
	${MAKE} -C markdown gcovr
	${MAKE} -C pdf gcovr
	${MAKE} -C smtp gcovr
	${MAKE} -C http gcovr
	${MAKE} -C httpd gcovr
	${MAKE} -C test gcovr
	${MAKE} -C window gcovr
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 gcovr; fi
	if [ -d "$$HOME/Downloads/kc3_gcovr" ]; then bin/gcovr-to-downloads; fi

gdb_demo: debug lib_links_debug
	${MAKE} -C libkc3 gdb_demo

gdb_demo_egl: debug lib_links_debug
	${MAKE} -C window/egl gdb_demo

gdb_demo_gl: debug lib_links_debug
	${MAKE} -C window/sdl2 gdb_demo

gdb_demo_kubz: debug lib_links_debug
	${MAKE} -C window/sdl2/kubz gdb_demo

gdb_dump: debug
	${MAKE} clean_dump
	gdb kc3s/kc3s_debug

gdb_fx: httpd_debug lib_links_debug
	${MAKE} -C httpd gdb_fx

gdb_fx_asan: httpd_asan lib_links_asan
	${MAKE} -C httpd gdb_fx_asan

gdb_ikc3: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 gdb_ikc3

gdb_ikc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 gdb_ikc3_asan

gdb_kmsg: lib_links_debug
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C gtk4 debug
	${MAKE} -C kmsg gdb

gdb_kubz: debug lib_links_debug
	${MAKE} -C window gdb_kubz

gdb_test: debug lib_links_debug
	${MAKE} -C test gdb_test

gdb_test_asan: asan lib_links_asan
	${MAKE} -C test gdb_test_asan

gdb_test_ekc3: debug lib_links_debug
	${MAKE} -C test gdb_test_ekc3

gdb_test_http: debug lib_links_debug
	${MAKE} -C test gdb_test_http

gdb_test_http_asan: asan lib_links_asan
	${MAKE} -C test gdb_test_http_asan

gdb_test_httpd: debug lib_links_debug
	${MAKE} -C test gdb_test_httpd

gdb_test_httpd_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C event asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C pdf asan
	${MAKE} -C smtp asan
	${MAKE} -C http asan
	${MAKE} -C httpd asan
	${MAKE} -C test gdb_test_httpd_asan

gdb_test_ikc3: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C test gdb_test_ikc3

gdb_test_ikc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C test gdb_test_ikc3_asan

gdb_test_json: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C json debug
	${MAKE} -C test gdb_test_json

gdb_test_json_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C json asan
	${MAKE} -C test gdb_test_json_asan

gdb_test_markdown: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C markdown debug
	${MAKE} -C test gdb_test_markdown

gdb_test_markdown_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C markdown asan
	${MAKE} -C test gdb_test_markdown_asan

gdb_test_pdf: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C pdf debug
	${MAKE} -C test gdb_test_pdf

gdb_test_pdf_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C pdf asan
	${MAKE} -C test gdb_test_pdf_asan

gdb_test_tls: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C test gdb_test_tls

gdb_test_tls_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C test gdb_test_tls_asan

gen:
	${MAKE} -C libkc3 gen

gtk4:
	${HAVE_GTK4} || echo "please install gtk+4"
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C gtk4 build

gtk4_asan:
	${HAVE_GTK4} || echo "please install gtk+4"
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C gtk4 asan

gtk4_cov:
	${HAVE_GTK4} || echo "please install gtk+4"
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C gtk4 cov

gtk4_debug:
	${HAVE_GTK4} || echo "please install gtk+4"
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C gtk4 debug

http:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C pdf build
	${MAKE} -C http build

httpd:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C pdf build
	${MAKE} -C http build
	${MAKE} -C httpd build

httpd_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C pdf debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug

ikc3:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build

ikc3_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan

ikc3_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov

ikc3_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug

ikc3_gcovr:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} clean_cov
	${MAKE} ikc3_test_cov
	${MAKE} gcovr

install:
	${INSTALL} -m 0755 -d ${DESTDIR}${libdir}/kc3
	${INSTALL} -m 0755 -d ${DESTDIR}${libdir}/kc3/0.1
	( cd ${SRC_DIR}/lib && \
	  find kc3/ -type d; ) | while read F; do \
	    ${INSTALL} -m 0755 -d ${DESTDIR}${libdir}/"$$F"; done
	${INSTALL} -m 0644 ${SRC_DIR}/lib/kc3/0.1/kc3.dump \
	    ${DESTDIR}${libdir}/kc3/0.1/kc3.dump
	( cd ${SRC_DIR}/lib && \
	  find kc3 -name '*.facts' -or -name '*.kc3'; ) | \
	      while read F; do ${INSTALL} -m 0644 "${SRC_DIR}/lib/$$F" \
		  "${DESTDIR}${libdir}/$$F"; done
	${INSTALL} -m 0755 -d ${DESTDIR}${prefix}/share/kc3
	( cd ${SRC_DIR} && find img -type d; ) | while read F; do \
	    ${INSTALL} -m 0755 -d ${DESTDIR}${prefix}/share/kc3/"$$F"; \
	done
	( cd ${SRC_DIR} && find img -name '*.png' -or -name '*.jpg'; ) | \
	    while read F; do ${INSTALL} -m 0644 "${SRC_DIR}/$$F" \
		      "${DESTDIR}${prefix}/share/kc3/$$F"; done
	${MAKE} -C libtommath install
	${MAKE} -C libkc3 install
	${MAKE} -C socket install
	${MAKE} -C tls install
	${MAKE} -C ikc3 install
	${MAKE} -C kc3s install
	${MAKE} -C kpkg install
	${MAKE} -C ekc3 install
	${MAKE} -C event install
	${MAKE} -C json install
	${MAKE} -C markdown install
	${MAKE} -C pdf install
	${MAKE} -C smtp install
	${MAKE} -C http install
	${MAKE} -C httpd install
	${MAKE} -C gl install
	${MAKE} -C window install
	if ${HAVE_GTK4}; then ${MAKE} -C gtk4 install; fi
	${MAKE} install_lib_links

install_lib_links:
	${MAKE} install_lib_links_${ARCH}

install_lib_links_bsd:
	ln -sf ../../libekc3.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/http.so
	ln -sf ../../libkc3_json.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/markdown.so
	ln -sf ../../libkc3_pdf.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/pdf.so
	ln -sf ../../libkc3_socket.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/socket.so
	ln -sf ../../libkc3_tls.so.0.0.0 ${DESTDIR}${libdir}/kc3/0.1/tls.so

install_lib_links_darwin:
	ln -sf ../../libekc3.0.dylib ${DESTDIR}${libdir}/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.0.dylib ${DESTDIR}${libdir}/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.0.dylib ${DESTDIR}${libdir}/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.0.dylib ${DESTDIR}${libdir}/kc3/0.1/http.so
	ln -sf ../../libkc3_json.0.dylib ${DESTDIR}${libdir}/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.0.dylib ${DESTDIR}${libdir}/kc3/0.1/markdown.so
	ln -sf ../../libkc3_pdf.0.dylib ${DESTDIR}${libdir}/kc3/0.1/pdf.so
	ln -sf ../../libkc3_smtp.so.0.dylib ${DESTDIR}${libdir}/kc3/0.1/smtp.so
	ln -sf ../../libkc3_socket.0.dylib ${DESTDIR}${libdir}/kc3/0.1/socket.so
	ln -sf ../../libkc3_tls.0.dylib ${DESTDIR}${libdir}/kc3/0.1/tls.so

install_lib_links_linux:
	ln -sf ../../libekc3.so ${DESTDIR}${libdir}/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.so ${DESTDIR}${libdir}/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.so ${DESTDIR}${libdir}/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.so ${DESTDIR}${libdir}/kc3/0.1/http.so
	ln -sf ../../libkc3_json.so ${DESTDIR}${libdir}/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.so ${DESTDIR}${libdir}/kc3/0.1/markdown.so
	ln -sf ../../libkc3_pdf.so ${DESTDIR}${libdir}/kc3/0.1/pdf.so
	ln -sf ../../libkc3_smtp.so ${DESTDIR}${libdir}/kc3/0.1/smtp.so
	ln -sf ../../libkc3_socket.so ${DESTDIR}${libdir}/kc3/0.1/socket.so
	ln -sf ../../libkc3_tls.so ${DESTDIR}${libdir}/kc3/0.1/tls.so

install_lib_links_openbsd:
	ln -sf ../../libekc3.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/http.so
	ln -sf ../../libkc3_json.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/markdown.so
	ln -sf ../../libkc3_pdf.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/pdf.so
	ln -sf ../../libkc3_smtp.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/smtp.so
	ln -sf ../../libkc3_socket.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/socket.so
	ln -sf ../../libkc3_tls.so.0.0 ${DESTDIR}${libdir}/kc3/0.1/tls.so

install_lib_links_windows:
	ln -sf ../../../bin/libekc3-0.dll ${DESTDIR}${libdir}/kc3/0.1/ekc3.so
	ln -sf ../../../bin/libkc3_event-0.dll ${DESTDIR}${libdir}/kc3/0.1/event.so
	ln -sf ../../../bin/libkc3_gtk4-0.dll ${DESTDIR}${libdir}/kc3/0.1/gtk4.so
	ln -sf ../../../bin/libkc3_http-0.dll ${DESTDIR}${libdir}/kc3/0.1/http.so
	ln -sf ../../../bin/libkc3_json-0.dll ${DESTDIR}${libdir}/kc3/0.1/json.so
	ln -sf ../../../bin/libkc3_markdown-0.dll ${DESTDIR}${libdir}/kc3/0.1/markdown.so
	ln -sf ../../../bin/libkc3_pdf-0.dll ${DESTDIR}${libdir}/kc3/0.1/pdf.so
	ln -sf ../../../bin/libkc3_smtp-0.dll ${DESTDIR}${libdir}/kc3/0.1/smtp.so
	ln -sf ../../../bin/libkc3_socket-0.dll ${DESTDIR}${libdir}/kc3/0.1/socket.so
	ln -sf ../../../bin/libkc3_tls-0.dll ${DESTDIR}${libdir}/kc3/0.1/tls.so

json:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C json build

json_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C json asan

json_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C json cov

json_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C json debug

kc3-${KC3_VERSION}.tar.gz: kc3.index
	rm -rf kc3-${KC3_VERSION}.old
	if [ -d kc3-${KC3_VERSION} ]; then \
		mv kc3-${KC3_VERSION} kc3-${KC3_VERSION}.old; \
	fi
	mkdir kc3-${KC3_VERSION}
	pax -rw < kc3.index kc3-${KC3_VERSION}
	pax -w kc3-${KC3_VERSION} | gzip -9 > kc3-${KC3_VERSION}.tar.gz

kc3.index: sources.mk Makefile
	for F in ${KC3_EXTERNAL_SOURCES}; do echo "$$F"; done > kc3.index.tmp
	for F in ${KC3_CONFIGURES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_MAKEFILES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_C_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_CXX_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_FONT_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_HTTPD_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_IMG_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_KMSG_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_LIB_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_OBJC_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_PLIST_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_SH_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_IKC3_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_EKC3_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_HTTP_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_HTTPD_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_JSON_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_KC3S_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_MARKDOWN_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_PDF_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_OTHER_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_DOC_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	sort -u < kc3.index.tmp > kc3.index
	rm kc3.index.tmp

kc3s:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C kc3s build

kc3s_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C kc3s asan

kc3s_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C kc3s cov

kc3s_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C kc3s debug

kmsg:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C gtk4 build
	${MAKE} -C kmsg

kmsg_asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C gtk4 asan
	${MAKE} -C kmsg run_asan

kmsg_debug:
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C gtk4 debug
	${MAKE} -C kmsg run_debug

kpkg:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C kpkg build

kpkg_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C kpkg asan

kpkg_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C kpkg cov

kpkg_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C kpkg debug

kubz:
	${MAKE} build
	window/sdl2/kubz/kubz

kubz_asan:
	${MAKE} asan
	window/sdl2/kubz/kubz_asan

kubz_debug:
	${MAKE} debug
	window/sdl2/kubz/kubz_debug

lib:
	if [ "x${SRC_TOP}" != "x${BUILD_TOP}" ]; then \
		rm -rf ${BUILD_TOP}/lib && \
		cd ${SRC_TOP} && \
		find lib -name '*.kc3' -or \
			 -name '*.facts' -or \
			 -name '*.dump' | \
			pax -rwd ${BUILD_TOP}/; fi

lib_links: lib
	${MAKE} lib_links_${ARCH}

lib_links_asan: lib
	${MAKE} lib_links_${ARCH}_asan

lib_links_bsd:
	ln -sf ../../../ekc3/libekc3.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../socket/libkc3_socket.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_bsd_asan:
	ln -sf ../../../ekc3/libekc3_asan.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_asan.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_asan.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_asan.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_asan.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_asan.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_asan.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../socket/libkc3_socket_asan.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_asan.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_bsd_debug:
	ln -sf ../../../ekc3/libekc3_debug.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_debug.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_debug.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_debug.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_debug.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_debug.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_debug.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../socket/libkc3_socket_debug.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_debug.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_cov: lib
	${MAKE} lib_links_${ARCH}_cov

lib_links_debug: lib
	${MAKE} lib_links_${ARCH}_debug

lib_links_darwin:
	ln -sf ../../../ekc3/libekc3.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_darwin_debug:
	ln -sf ../../../ekc3/libekc3_debug.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_debug.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_debug.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_debug.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_debug.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_debug.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_debug.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp_debug.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket_debug.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_debug.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_linux:
	ln -sf ../../../ekc3/libekc3.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_linux_asan:
	ln -sf ../../../ekc3/libekc3_asan.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_asan.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_asan.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_asan.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_asan.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_asan.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_asan.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp_asan.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket_asan.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_asan.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_linux_cov:
	ln -sf ../../../ekc3/libekc3_cov.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_cov.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_cov.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_cov.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_cov.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_cov.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_cov.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp_cov.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket_cov.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_cov.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_linux_debug:
	ln -sf ../../../ekc3/libekc3_debug.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_debug.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_debug.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_debug.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_debug.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_debug.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_debug.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp_debug.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket_debug.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_debug.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_openbsd:
	ln -sf ../../../ekc3/libekc3.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_openbsd_debug:
	ln -sf ../../../ekc3/libekc3_debug.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_debug.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_debug.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_debug.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_debug.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_debug.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_debug.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp_debug.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket_debug.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_debug.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_clean:
	rm -f lib/kc3/0.1/ekc3.so
	rm -f lib/kc3/0.1/event.so
	rm -f lib/kc3/0.1/gtk4.so
	rm -f lib/kc3/0.1/http.so
	rm -f lib/kc3/0.1/json.so
	rm -f lib/kc3/0.1/markdown.so
	rm -f lib/kc3/0.1/pdf.so
	rm -f lib/kc3/0.1/smtp.so
	rm -f lib/kc3/0.1/socket.so
	rm -f lib/kc3/0.1/tls.so

lib_links_windows:
	ln -sf ../../../ekc3/libekc3.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls.${SHARED_EXT} lib/kc3/0.1/tls.so

lib_links_windows_debug:
	ln -sf ../../../ekc3/libekc3_debug.${SHARED_EXT} lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/libkc3_event_debug.${SHARED_EXT} lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/libkc3_gtk4_debug.${SHARED_EXT} lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/libkc3_http_debug.${SHARED_EXT} lib/kc3/0.1/http.so
	ln -sf ../../../json/libkc3_json_debug.${SHARED_EXT} lib/kc3/0.1/json.so
	ln -sf ../../../markdown/libkc3_markdown_debug.${SHARED_EXT} lib/kc3/0.1/markdown.so
	ln -sf ../../../pdf/libkc3_pdf_debug.${SHARED_EXT} lib/kc3/0.1/pdf.so
	ln -sf ../../../smtp/libkc3_smtp_debug.${SHARED_EXT} lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/libkc3_socket_debug.${SHARED_EXT} lib/kc3/0.1/socket.so
	ln -sf ../../../tls/libkc3_tls_debug.${SHARED_EXT} lib/kc3/0.1/tls.so

libkc3_gcovr:
	${MAKE} clean_cov
	${MAKE} libkc3_test_cov
	${MAKE} gcovr

license:
	update_header config.subr ${KC3_CONFIGURES}
	update_header Makefile ${KC3_MAKEFILES}
	update_header libkc3/license.c ${KC3_C_SOURCES}

lldb_demo:
	${MAKE} debug
	${MAKE} -C window/cairo lldb_demo

lldb_demo_gl:
	${MAKE} debug
	${MAKE} -C window/sdl2 lldb_demo

lldb_ikc3:
	${MAKE} debug
	${MAKE} -C ikc3 lldb_ikc3

lldb_test:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C kpkg debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C pdf debug
	${MAKE} -C smtp debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test lldb_test

markdown:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C markdown build

markdown_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C markdown asan

markdown_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C markdown cov

markdown_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C markdown debug

pdf:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C pdf build

pdf_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C pdf asan

pdf_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C pdf cov

pdf_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C pdf debug

release:
	mkdir -p release/v${VER}
	cp ${DIST}.tar.gz release/v${VER}

test: build lib_links
	${MAKE} -C test test
	if ${HAVE_ASAN}; then ${MAKE} test_asan; fi

test_asan: asan lib_links_asan
	${MAKE} -C test test_asan

test_cov: cov lib_links_cov
	${MAKE} -C test test_cov

test_debug: debug lib_links_debug
	${MAKE} -C test test_debug

test_ekc3: ekc3 lib_links
	${MAKE} -C test test_ekc3

test_ekc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C test test_ekc3_asan

test_ekc3_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov
	${MAKE} -C test test_ekc3_cov

test_ekc3_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C test test_ekc3_debug

test_gcovr: lib_links_cov
	${MAKE} clean_cov
	${MAKE} test_libkc3_cov
	${MAKE} test_ikc3_cov
	${MAKE} test_ekc3_cov
	${MAKE} test_tls_cov
	${MAKE} gcovr

test_http: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C pdf build
	${MAKE} -C smtp build
	${MAKE} -C http build
	${MAKE} -C test test_http

test_http_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C pdf asan
	${MAKE} -C smtp asan
	${MAKE} -C http asan
	${MAKE} -C test test_http_asan

test_http_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C json cov
	${MAKE} -C markdown cov
	${MAKE} -C pdf cov
	${MAKE} -C smtp cov
	${MAKE} -C http cov
	${MAKE} -C test test_http_cov

test_http_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C pdf debug
	${MAKE} -C http debug
	${MAKE} -C test test_http_debug

test_httpd: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C pdf build
	${MAKE} -C http build
	${MAKE} -C httpd build
	${MAKE} -C test test_httpd

test_httpd_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C event asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C pdf asan
	${MAKE} -C http asan
	${MAKE} -C httpd asan
	${MAKE} -C test test_httpd_asan

test_httpd_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov
	${MAKE} -C event cov
	${MAKE} -C json cov
	${MAKE} -C markdown cov
	${MAKE} -C pdf cov
	${MAKE} -C http cov
	${MAKE} -C httpd cov
	${MAKE} -C test test_httpd_cov

test_httpd_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C pdf debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test test_httpd_debug

test_ikc3: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C test test_ikc3

test_ikc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C test test_ikc3_asan

test_ikc3_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C test test_ikc3_cov

test_ikc3_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C test test_ikc3_debug

test_json: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C json build
	${MAKE} -C test test_json

test_json_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C json asan
	${MAKE} -C test test_json_asan

test_json_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C json cov
	${MAKE} -C test test_json_cov

test_json_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C json debug
	${MAKE} -C test test_json_debug

test_kc3s: kc3s lib_links
	${MAKE} -C test test_kc3s

test_kc3s_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C test test_kc3s_asan

test_kc3s_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C test test_kc3s_cov

test_kc3s_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C test test_kc3s_debug

test_kpkg: kpkg lib_links
	${MAKE} -C test test_kpkg

test_kpkg_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C kpkg asan
	${MAKE} -C test test_kpkg_asan

test_kpkg_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C kpkg cov
	${MAKE} -C test test_kpkg_cov

test_kpkg_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C kpkg debug
	${MAKE} -C test test_kpkg_debug

test_libkc3: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C test test_libkc3

test_libkc3_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C test test_libkc3_cov

test_libkc3_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C test test_libkc3_debug

test_markdown: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C markdown build
	${MAKE} -C test test_markdown

test_markdown_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C markdown asan
	${MAKE} -C test test_markdown_asan

test_markdown_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C markdown cov
	${MAKE} -C test test_markdown_cov

test_markdown_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C markdown debug
	${MAKE} -C test test_markdown_debug

test_pdf: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C pdf build
	${MAKE} -C test test_pdf

test_pdf_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C pdf asan
	${MAKE} -C test test_pdf_asan

test_pdf_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C pdf cov
	${MAKE} -C test test_pdf_cov

test_pdf_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C pdf debug
	${MAKE} -C test test_pdf_debug

test_socket: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C test test_socket

test_socket_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C test test_socket_asan

test_socket_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C test test_socket_cov

test_socket_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C test test_socket_debug

test_tls: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C test test_tls

test_tls_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C socket asan
	${MAKE} -C tls asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C test test_tls_asan

test_tls_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C socket cov
	${MAKE} -C tls cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C test test_tls_cov

test_tls_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C libkc3 debug
	${MAKE} -C socket debug
	${MAKE} -C tls debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C test test_tls_debug

tls:
	${MAKE} -C libtommath build
	${MAKE} -C libkc3 build
	${MAKE} -C socket build
	${MAKE} -C tls build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build

uninstall:
	rm -rf ${prefix}/lib/kc3
	rm -rf ${prefix}/share/kc3
	${MAKE} -C libtommath uninstall
	${MAKE} -C libkc3 uninstall
	${MAKE} -C socket uninstall
	${MAKE} -C tls uninstall
	${MAKE} -C ikc3 uninstall
	${MAKE} -C kc3s uninstall
	${MAKE} -C kpkg uninstall
	${MAKE} -C ekc3 uninstall
	${MAKE} -C event uninstall
	${MAKE} -C json uninstall
	${MAKE} -C markdown uninstall
	${MAKE} -C pdf uninstall
	${MAKE} -C smtp uninstall
	${MAKE} -C http uninstall
	${MAKE} -C httpd uninstall
	${MAKE} -C window uninstall
	${MAKE} -C gtk4 uninstall

.PHONY: all \
	android \
	asan \
	assets \
	build \
	check \
	cov \
	clean \
	clean_cov \
	clean_dump \
	deb \
	debug \
	demo \
	demo_asan \
	demo_cov \
	demo_debug \
	demo_gl \
	demo_gl_asan \
	demo_gl_cov \
	demo_gl_debug \
	dist \
	dump \
	ekc3 \
	ekc3_asan \
	ekc3_cov \
	ekc3_debug \
	event \
	event_asan \
	event_cov \
	event_debug \
	fx \
	fx_asan \
	fx_cov \
	fx_debug \
	gcovr \
	gdb_fx \
	gdb_fx_asan \
	gdb_test \
	gdb_test_asan \
	gdb_test_debug \
	gdb_test_gcovr \
	gdb_test_ekc3 \
	gdb_test_ekc3_asan \
	gdb_test_ekc3_debug \
	gdb_test_http \
	gdb_test_http_asan \
	gdb_test_http_debug \
	gdb_test_httpd \
	gdb_test_httpd_asan \
	gdb_test_httpd_debug \
	gdb_test_ikc3 \
	gdb_test_ikc3_asan \
	gdb_test_ikc3_debug \
	gdb_test_json \
	gdb_test_json_asan \
	gdb_test_json_debug \
	gdb_test_kc3s \
	gdb_test_kc3s_asan \
	gdb_test_kc3s_debug \
	gdb_test_libkc3 \
	gdb_test_libkc3_asan \
	gdb_test_libkc3_debug \
	gdb_test_markdown \
	gdb_test_markdown_asan \
	gdb_test_markdown_debug \
	gdb_test_pdf \
	gdb_test_pdf_asan \
	gdb_test_pdf_debug \
	gdb_test_socket \
	gdb_test_socket_asan \
	gdb_test_socket_debug \
	gdb_test_tls \
	gdb_test_tls_asan \
	gdb_test_tls_debug \
	gtk4 \
	gtk4_asan \
	gtk4_cov \
	gtk4_debug \
	http \
	http_asan \
	http_cov \
	http_debug \
	httpd \
	httpd_asan \
	httpd_cov \
	httpd_debug \
	ikc3 \
	ikc3_asan \
	ikc3_cov \
	ikc3_debug \
	install \
	json \
	json_asan \
	json_cov \
	json_debug \
	kc3.index \
	kc3s \
	kc3s_asan \
	kc3s_cov \
	kc3s_debug \
	kmsg \
	kmsg_asan \
	kmsg_cov \
	kmsg_debug \
	kpkg \
	kpkg_asan \
	kpkg_cov \
	kpkg_debug \
	lib \
	lib_links \
	lib_links_asan \
	lib_links_bsd \
	lib_links_bsd_asan \
	lib_links_bsd_debug \
	lib_links_cov \
	lib_links_debug \
	lib_links_linux \
	lib_links_linux_asan \
	lib_links_linux_debug \
	libkc3 \
	libtommath \
	license \
	markdown \
	markdown_asan \
	markdown_cov \
	markdown_debug \
	pdf \
	pdf_asan \
	pdf_cov \
	pdf_debug \
	release \
	socket \
	socket_asan \
	socket_cov \
	socket_debug \
	test \
	test_asan \
	test_cov \
	test_debug \
	test_gcovr \
	test_ekc3 \
	test_ekc3_asan \
	test_ekc3_cov \
	test_ekc3_debug \
	test_http \
	test_http_asan \
	test_http_cov \
	test_http_debug \
	test_httpd \
	test_httpd_asan \
	test_httpd_cov \
	test_httpd_debug \
	test_ikc3 \
	test_ikc3_asan \
	test_ikc3_cov \
	test_ikc3_debug \
	test_json \
	test_json_asan \
	test_json_cov \
	test_json_debug \
	test_kc3s \
	test_kc3s_asan \
	test_kc3s_cov \
	test_kc3s_debug \
	test_libkc3 \
	test_libkc3_asan \
	test_libkc3_cov \
	test_libkc3_debug \
	test_markdown \
	test_markdown_asan \
	test_markdown_cov \
	test_markdown_debug \
	test_pdf \
	test_pdf_asan \
	test_pdf_cov \
	test_pdf_debug \
	test_socket \
	test_socket_asan \
	test_socket_cov \
	test_socket_debug \
	test_tls \
	test_tls_asan \
	test_tls_cov \
	test_tls_debug \
	tls \
	uninstall
