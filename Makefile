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
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 all
	${MAKE} -C ikc3 all
	${MAKE} -C kc3s all
	${MAKE} -C ekc3 all
	${MAKE} -C event all
	${MAKE} -C json all
	${MAKE} -C markdown all
	${MAKE} -C smtp all
	${MAKE} -C socket all
	${MAKE} -C http all
	${MAKE} -C httpd all
	${MAKE} -C test all
	${MAKE} -C window all
	${MAKE} -C gtk4 all
#	${MAKE} -C fs all

include config.mk
include sources.mk

asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C event asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C smtp asan
	${MAKE} -C socket asan
	${MAKE} -C http asan
	${MAKE} -C httpd asan
	${MAKE} -C test asan
	${MAKE} -C window asan
	${MAKE} -C gtk4 asan
#	${MAKE} -C fs asan

assets:
	make -C test/httpd/assets

build:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C smtp build
	${MAKE} -C socket build
	${MAKE} -C http build
	${MAKE} -C httpd build
	${MAKE} -C test build
	${MAKE} -C window build
	${MAKE} -C gtk4 build
#	${MAKE} -C fs build

clean:
	${MAKE} -C libtommath clean
	${MAKE} -C ucd2c clean
	${MAKE} -C libkc3 clean
	${MAKE} -C ikc3 clean
	${MAKE} -C kc3s clean
	${MAKE} -C ekc3 clean
	${MAKE} -C event clean
	${MAKE} -C json clean
	${MAKE} -C markdown clean
	${MAKE} -C smtp clean
	${MAKE} -C socket clean
	${MAKE} -C http clean
	${MAKE} -C httpd clean
	${MAKE} -C test clean
	${MAKE} -C window clean
	${MAKE} -C gtk4 clean
#	${MAKE} -C fs clean

clean_cov:
	${MAKE} -C libtommath clean_cov
	${MAKE} -C libkc3 clean_cov
	${MAKE} -C ikc3 clean_cov
	${MAKE} -C kc3s clean_cov
	${MAKE} -C ekc3 clean_cov
	${MAKE} -C event clean_cov
	${MAKE} -C json clean_cov
	${MAKE} -C markdown clean_cov
	${MAKE} -C smtp clean_cov
	${MAKE} -C socket clean_cov
	${MAKE} -C http clean_cov
	${MAKE} -C httpd clean_cov
	${MAKE} -C test clean_cov
	${MAKE} -C window clean_cov
	${MAKE} -C gtk4 clean_cov
#	${MAKE} -C fs clean_cov

cov:
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov
	${MAKE} -C event cov
	${MAKE} -C json cov
	${MAKE} -C markdown cov
	${MAKE} -C smtp cov
	${MAKE} -C socket cov
	${MAKE} -C http cov
	${MAKE} -C httpd cov
	${MAKE} -C test cov
	${MAKE} -C window cov
	${MAKE} -C gtk4 cov
#	${MAKE} -C fs cov

debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C smtp debug
	${MAKE} -C socket debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test debug
	${MAKE} -C window debug
	${MAKE} -C gtk4 debug
#	${MAKE} -C fs debug

demo: build
	${MAKE} -C window demo

demo_asan: asan
	${MAKE} -C window demo_asan

demo_cov: cov
	${MAKE} -C window demo_cov

demo_debug: debug
	${MAKE} -C window demo_debug

demo_gl:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C window demo_gl

demo_gl_asan: asan
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C window demo_gl_asan

demo_gl_cov: cov
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C window demo_gl_cov

demo_gl_debug: debug
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C window demo_gl_debug

demo_kubz: build
	${MAKE} -C window/sdl2/kubz demo

demo_kubz_debug: debug
	${MAKE} -C window/sdl2/kubz demo_debug

dist: kc3-${KC3_VERSION}.tar.gz

dist_msys2_clang64: all
	${MAKE} -C msys2/clang64

distclean:
	${MAKE} -C libtommath distclean
	${MAKE} -C ucd2c distclean
	${MAKE} -C libkc3 distclean
	${MAKE} -C ikc3 distclean
	${MAKE} -C kc3s distclean
	${MAKE} -C ekc3 distclean
	${MAKE} -C event distclean
	${MAKE} -C json distclean
	${MAKE} -C markdown distclean
	${MAKE} -C smtp distclean
	${MAKE} -C socket distclean
	${MAKE} -C http distclean
	${MAKE} -C httpd distclean
	${MAKE} -C test distclean
	${MAKE} -C window distclean
	${MAKE} -C gtk4 distclean
#	${MAKE} -C fs distclean

clean_dump:
	rm -f lib/kc3/0.1/kc3.dump test/httpd/kc3.dump

dump:
	${MAKE} clean_dump
	kc3s/kc3s --trace --dump lib/kc3/0.1/kc3.dump --quit
	${MAKE} -C test/httpd dump

dump_debug:
	${MAKE} clean_dump
	kc3s/kc3s_debug --trace --dump lib/kc3/0.1/kc3.dump --quit
	${MAKE} -C test/httpd dump_debug

ekc3:
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build

ekc3_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan

ekc3_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov

ekc3_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug

event:
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C event build

event_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C event asan

event_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C event cov

event_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C event debug

fx: httpd fx_assets
	${PWD}/httpd/.libs/kc3_httpd -C httpd/fx -d 127.0.0.1 58001

fx_asan: httpd_asan fx_assets
	${PWD}/httpd/.libs/kc3_httpd_asan -C httpd/fx -d 127.0.0.1 58001

fx_assets:
	${MAKE} -C httpd/fx/assets

fx_cov: httpd_cov fx_assets
	${PWD}/httpd/.libs/kc3_httpd_cov -C httpd/fx -d 127.0.0.1 58001

fx_debug: httpd_debug fx_assets
	${PWD}/httpd/.libs/kc3_httpd_debug -C httpd/fx -d 127.0.0.1 58001

gcovr:
	${MAKE} -C libkc3 gcovr
	${MAKE} -C ikc3 gcovr
	${MAKE} -C kc3s gcovr
	${MAKE} -C ekc3 gcovr
	${MAKE} -C event gcovr
	${MAKE} -C json gcovr
	${MAKE} -C markdown gcovr
	${MAKE} -C smtp gcovr
	${MAKE} -C socket gcovr
	${MAKE} -C http gcovr
	${MAKE} -C httpd gcovr
	${MAKE} -C test gcovr
	${MAKE} -C window gcovr
	${MAKE} -C gtk4 gcovr
#	${MAKE} -C fs gcovr
	if [ -d "$$HOME/Downloads/kc3_gcovr" ]; then bin/gcovr-to-downloads; fi

gdb_demo: lib_links_debug
	${MAKE} debug
	${MAKE} -C libkc3 gdb_demo

gdb_demo_gl: lib_links_debug
	${MAKE} debug
	${MAKE} -C window/sdl2 gdb_demo

gdb_demo_kubz: lib_links_debug
	${MAKE} debug
	${MAKE} -C window/sdl2/kubz gdb_demo

gdb_dump: debug
	${MAKE} clean_dump
	gdb kc3s/.libs/kc3s_debug

gdb_fx: httpd_debug lib_links_debug
	${MAKE} -C httpd gdb_fx

gdb_fx_asan: httpd_asan lib_links_asan
	${MAKE} -C httpd gdb_fx_asan

gdb_ikc3: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 gdb_ikc3

gdb_ikc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 gdb_ikc3_asan

gdb_kmsg: lib_links_debug
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C gtk4 debug
	${MAKE} -C kmsg gdb

gdb_kubz: debug lib_links_debug
	${MAKE} -C window gdb_kubz

gdb_test: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C test gdb_test

gdb_test_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C test gdb_test_asan

gdb_test_ekc3: lib_links_debugOA
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C test gdb_test_ekc3

gdb_test_http: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C smtp debug
	${MAKE} -C socket debug
	${MAKE} -C http debug
	${MAKE} -C test gdb_test_http

gdb_test_http_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C smtp asan
	${MAKE} -C socket asan
	${MAKE} -C http asan
	${MAKE} -C test gdb_test_http_asan

gdb_test_httpd: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C smtp debug
	${MAKE} -C socket debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test gdb_test_httpd

gdb_test_httpd_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C event asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C smtp asan
	${MAKE} -C socket asan
	${MAKE} -C http asan
	${MAKE} -C httpd asan
	${MAKE} -C test gdb_test_httpd_asan

gdb_test_ikc3: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C test gdb_test_ikc3

gdb_test_ikc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C test gdb_test_ikc3_asan

gdb_test_json: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C json debug
	${MAKE} -C test gdb_test_json

gdb_test_json_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C json asan
	${MAKE} -C test gdb_test_json_asan

gdb_test_markdown: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C markdown debug
	${MAKE} -C test gdb_test_markdown

gdb_test_markdown_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C markdown asan
	${MAKE} -C test gdb_test_markdown_asan

gen:
	${MAKE} -C libkc3 gen

gtk4:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C gtk4 build

gtk4_asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C gtk4 asan

gtk4_cov:
	${MAKE} gen
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C gtk4 cov

gtk4_debug:
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C gtk4 debug

http:
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C socket build
	${MAKE} -C http build

httpd:
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C socket build
	${MAKE} -C http build
	${MAKE} -C httpd build

httpd_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C socket debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug

ikc3:
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build

ikc3_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan

ikc3_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov

ikc3_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug

ikc3_gcovr:
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} clean_cov
	${MAKE} ikc3_test_cov
	${MAKE} gcovr

install: lib_links_debug
	${INSTALL} -m 0755 -d ${prefix}/lib/kc3
	${INSTALL} -m 0755 -d ${prefix}/lib/kc3/0.1
	find lib/kc3/ -type d | while read F; do \
	    ${INSTALL} -m 0755 -d ${prefix}/"$$F"; done
	${INSTALL} -m 0644 lib/kc3/0.1/kc3.dump \
	    ${prefix}/lib/kc3/0.1/kc3.dump
	find lib -name '*.facts' -or -name '*.kc3' | while read F; do \
	    ${INSTALL} -m 0644 $$F ${prefix}/"$$F"; done
	${INSTALL} -m 0755 -d ${prefix}/share/kc3
	${INSTALL} -m 0755 -d ${prefix}/share/kc3/img
	find img/*/ -type d | while read F; do \
	    ${INSTALL} -m 0755 -d ${prefix}/share/kc3/"$$F"; done
	find img -name '*.png' -or -name '*.jpg' | while read F; do \
	    ${INSTALL} -m 0644 "$$F" ${prefix}/share/kc3/"$$F"; done
	${MAKE} -C libtommath install
	${MAKE} -C libkc3 install
	${MAKE} -C ikc3 install
	${MAKE} -C kc3s install
	${MAKE} -C ekc3 install
	${MAKE} -C event install
	${MAKE} -C json install
	${MAKE} -C markdown install
	${MAKE} -C smtp install
	${MAKE} -C socket install
	${MAKE} -C http install
	${MAKE} -C httpd install
	${MAKE} -C window install
	${MAKE} -C gtk4 install
#	${MAKE} -C fs install
	${MAKE} install_lib_links

install_lib_links:
	${MAKE} install_lib_links_${ARCH}

install_lib_links_bsd:
	ln -sf ../../libekc3.so.0.0 ${prefix}/lib/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.so.0.0 ${prefix}/lib/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.so.0.0 ${prefix}/lib/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.so.0.0 ${prefix}/lib/kc3/0.1/http.so
	ln -sf ../../libkc3_json.so.0.0 ${prefix}/lib/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.so.0.0 ${prefix}/lib/kc3/0.1/markdown.so
	ln -sf ../../libkc3_smtp.so.0.0 ${prefix}/lib/kc3/0.1/smtp.so
	ln -sf ../../libkc3_socket.so.0.0 ${prefix}/lib/kc3/0.1/socket.so

install_lib_links_darwin:
	ln -sf ../../libekc3.0.dylib ${prefix}/lib/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.0.dylib ${prefix}/lib/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.0.dylib ${prefix}/lib/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.0.dylib ${prefix}/lib/kc3/0.1/http.so
	ln -sf ../../libkc3_json.0.dylib ${prefix}/lib/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.0.dylib ${prefix}/lib/kc3/0.1/markdown.so
	ln -sf ../../libkc3_smtp.so.0.dylib ${prefix}/lib/kc3/0.1/smtp.so
	ln -sf ../../libkc3_socket.0.dylib ${prefix}/lib/kc3/0.1/socket.so

install_lib_links_freebsd:
	ln -sf ../../libekc3.so.0.0.0 ${prefix}/lib/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.so.0.0.0 ${prefix}/lib/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.so.0.0.0 ${prefix}/lib/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.so.0.0.0 ${prefix}/lib/kc3/0.1/http.so
	ln -sf ../../libkc3_json.so.0.0.0 ${prefix}/lib/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.so.0.0.0 ${prefix}/lib/kc3/0.1/markdown.so
	ln -sf ../../libkc3_socket.so.0.0.0 ${prefix}/lib/kc3/0.1/socket.so

install_lib_links_linux:
	ln -sf ../../libekc3.so ${prefix}/lib/kc3/0.1/ekc3.so
	ln -sf ../../libkc3_event.so ${prefix}/lib/kc3/0.1/event.so
	ln -sf ../../libkc3_gtk4.so ${prefix}/lib/kc3/0.1/gtk4.so
	ln -sf ../../libkc3_http.so ${prefix}/lib/kc3/0.1/http.so
	ln -sf ../../libkc3_json.so ${prefix}/lib/kc3/0.1/json.so
	ln -sf ../../libkc3_markdown.so ${prefix}/lib/kc3/0.1/markdown.so
	ln -sf ../../libkc3_smtp.so ${prefix}/lib/kc3/0.1/smtp.so
	ln -sf ../../libkc3_socket.so ${prefix}/lib/kc3/0.1/socket.so

install_lib_links_windows:
	ln -sf ../../../bin/libekc3-0.dll ${prefix}/lib/kc3/0.1/ekc3.so
	ln -sf ../../../bin/libkc3_event-0.dll ${prefix}/lib/kc3/0.1/event.so
	ln -sf ../../../bin/libkc3_gtk4-0.dll ${prefix}/lib/kc3/0.1/gtk4.so
	ln -sf ../../../bin/libkc3_http-0.dll ${prefix}/lib/kc3/0.1/http.so
	ln -sf ../../../bin/libkc3_json-0.dll ${prefix}/lib/kc3/0.1/json.so
	ln -sf ../../../bin/libkc3_markdown-0.dll ${prefix}/lib/kc3/0.1/markdown.so
	ln -sf ../../../bin/libkc3_smtp-0.dll ${prefix}/lib/kc3/0.1/smtp.so
	ln -sf ../../../bin/libkc3_socket-0.dll ${prefix}/lib/kc3/0.1/socket.so

json:
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C json build

json_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C json asan

json_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C json cov

json_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C json debug

kc3-${KC3_VERSION}.tar.gz: kc3.index
	rm -rf kc3-${KC3_VERSION}.old
	-mv kc3-${KC3_VERSION} kc3-${KC3_VERSION}.old
	mkdir kc3-${KC3_VERSION}
	pax -rw < kc3.index kc3-${KC3_VERSION}
	pax -wz kc3-${KC3_VERSION} > kc3-${KC3_VERSION}.tar.gz

kc3.index: sources.mk Makefile
	echo -n > kc3.index.tmp
	for F in ${KC3_EXTERNAL_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
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
	for F in ${KC3_TEST_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_IKC3_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_EKC3_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_HTTP_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_HTTPD_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_JSON_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_KC3S_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_TEST_MARKDOWN_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_OTHER_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	for F in ${KC3_DOC_SOURCES}; do echo "$$F"; done >> kc3.index.tmp
	sort -u < kc3.index.tmp > kc3.index
	rm kc3.index.tmp

kmsg:
	${MAKE} gen
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C gtk4 build
	${MAKE} -C kmsg

kmsg_asan:
	${MAKE} gen
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C gtk4 asan
	${MAKE} -C kmsg run_asan

kmsg_debug:
	${MAKE} gen
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C gtk4 debug
	${MAKE} -C kmsg run_debug

kubz:
	${MAKE} build
	window/sdl2/kubz/.libs/kubz

kubz_asan:
	${MAKE} asan
	window/sdl2/kubz/.libs/kubz_asan

kubz_debug:
	${MAKE} debug
	window/sdl2/kubz/.libs/kubz_debug

lib_links:
	${MAKE} lib_links_${ARCH}

lib_links_asan:
	${MAKE} lib_links_${ARCH}_asan

lib_links_cov:
	${MAKE} lib_links_${ARCH}_cov

lib_links_debug:
	${MAKE} lib_links_${ARCH}_debug

lib_links_bsd:
	ln -sf ../../../ekc3/.libs/libekc3.so.0.0 lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event.so.0.0 lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4.so.0.0 lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http.so.0.0 lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json.so.0.0 lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown.so.0.0 lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp.so.0.0 lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket.so.0.0 lib/kc3/0.1/socket.so

lib_links_bsd_debug:
	ln -sf ../../../ekc3/.libs/libekc3_debug.so.0.0 lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_debug.so.0.0 lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_debug.so.0.0 lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_debug.so.0.0 lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_debug.so.0.0 lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_debug.so.0.0 lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp_debug.so.0.0 lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket_debug.so.0.0 lib/kc3/0.1/socket.so

lib_links_clean:
	rm lib/kc3/0.1/ekc3.so
	rm lib/kc3/0.1/event.so
	rm lib/kc3/0.1/gtk4.so
	rm lib/kc3/0.1/http.so
	rm lib/kc3/0.1/json.so
	rm lib/kc3/0.1/markdown.so
	rm lib/kc3/0.1/smtp.so
	rm lib/kc3/0.1/socket.so

lib_links_darwin:
	ln -sf ../../../ekc3/.libs/libekc3.0.dylib lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event.0.dylib lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4.0.dylib lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http.0.dylib lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json.0.dylib lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown.0.dylib lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp.so.0.dylib lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket.0.dylib lib/kc3/0.1/socket.so

lib_links_darwin_debug:
	ln -sf ../../../ekc3/.libs/libekc3_debug.0.dylib lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_debug.0.dylib lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_debug.0.dylib lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_debug.0.dylib lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_debug.0.dylib lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_debug.0.dylib lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp_debug.so.0.dylib lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket_debug.0.dylib lib/kc3/0.1/socket.so

lib_links_freebsd:
	ln -sf ../../../ekc3/.libs/libekc3.so.0.0.0 lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event.so.0.0.0 lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4.so.0.0.0 lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http.so.0.0.0 lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json.so.0.0.0 lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown.so.0.0.0 lib/kc3/0.1/markdown.so
	ln -sf ../../../socket/.libs/libkc3_socket.so.0.0.0 lib/kc3/0.1/socket.so

lib_links_freebsd_debug:
	ln -sf ../../../ekc3/.libs/libekc3_debug.so.0.0.0 lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_debug.so.0.0.0 lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_debug.so.0.0.0 lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_debug.so.0.0.0 lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_debug.so.0.0.0 lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_debug.so.0.0.0 lib/kc3/0.1/markdown.so
	ln -sf ../../../socket/.libs/libkc3_socket_debug.so.0.0.0 lib/kc3/0.1/socket.so

lib_links_linux:
	ln -sf ../../../ekc3/.libs/libekc3.so lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event.so lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4.so lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http.so lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json.so lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown.so lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp.so lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket.so lib/kc3/0.1/socket.so

lib_links_linux_asan:
	ln -sf ../../../ekc3/.libs/libekc3_asan.so lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_asan.so lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_asan.so lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_asan.so lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_asan.so lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_asan.so lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp_asan.so lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket_asan.so lib/kc3/0.1/socket.so

lib_links_linux_cov:
	ln -sf ../../../ekc3/.libs/libekc3_cov.so lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_cov.so lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_cov.so lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_cov.so lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_cov.so lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_cov.so lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp_cov.so lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket_cov.so lib/kc3/0.1/socket.so

lib_links_linux_debug:
	ln -sf ../../../ekc3/.libs/libekc3_debug.so lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_debug.so lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_debug.so lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_debug.so lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_debug.so lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_debug.so lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp_debug.so lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket_debug.so lib/kc3/0.1/socket.so

lib_links_windows:
	ln -sf ../../../ekc3/.libs/libekc3-0.dll lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event-0.dll lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4-0.dll lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http-0.dll lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json-0.dll lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown-0.dll lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp-0.dll lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket-0.dll lib/kc3/0.1/socket.so

lib_links_windows_debug:
	ln -sf ../../../ekc3/.libs/libekc3_debug-0.dll lib/kc3/0.1/ekc3.so
	ln -sf ../../../event/.libs/libkc3_event_debug-0.dll lib/kc3/0.1/event.so
	ln -sf ../../../gtk4/.libs/libkc3_gtk4_debug-0.dll lib/kc3/0.1/gtk4.so
	ln -sf ../../../http/.libs/libkc3_http_debug-0.dll lib/kc3/0.1/http.so
	ln -sf ../../../json/.libs/libkc3_json_debug-0.dll lib/kc3/0.1/json.so
	ln -sf ../../../markdown/.libs/libkc3_markdown_debug-0.dll lib/kc3/0.1/markdown.so
	ln -sf ../../../smtp/.libs/libkc3_smtp_debug-0.dll lib/kc3/0.1/smtp.so
	ln -sf ../../../socket/.libs/libkc3_socket_debug-0.dll lib/kc3/0.1/socket.so

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
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C socket debug
	${MAKE} -C smtp debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test lldb_test

markdown:
	${MAKE} -C libtommath buildOA
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C markdown build

markdown_asan:
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C markdown asan

markdown_cov:
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C markdown cov

markdown_debug:
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C markdown debug

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
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C test test_ekc3_asan

test_ekc3_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov
	${MAKE} -C test test_ekc3_cov

test_ekc3_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C test test_ekc3_debug

test_gcovr: lib_links_cov
	${MAKE} clean_cov
	${MAKE} test_libkc3_cov
	${MAKE} test_ikc3_cov
	${MAKE} test_ekc3_cov
	${MAKE} gcovr

test_http: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C smtp build
	${MAKE} -C socket build
	${MAKE} -C http build
	${MAKE} -C test test_http

test_http_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C smtp asan
	${MAKE} -C socket asan
	${MAKE} -C http asan
	${MAKE} -C test test_http_asan

test_http_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C json cov
	${MAKE} -C markdown cov
	${MAKE} -C smtp cov
	${MAKE} -C socket cov
	${MAKE} -C http cov
	${MAKE} -C test test_http_cov

test_http_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C socket debug
	${MAKE} -C http debug
	${MAKE} -C test test_http_debug

test_httpd: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C ekc3 build
	${MAKE} -C event build
	${MAKE} -C json build
	${MAKE} -C markdown build
	${MAKE} -C socket build
	${MAKE} -C http build
	${MAKE} -C httpd build
	${MAKE} -C test test_httpd

test_httpd_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C ekc3 asan
	${MAKE} -C event asan
	${MAKE} -C json asan
	${MAKE} -C markdown asan
	${MAKE} -C socket asan
	${MAKE} -C http asan
	${MAKE} -C httpd asan
	${MAKE} -C test test_httpd_asan

test_httpd_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C ekc3 cov
	${MAKE} -C event cov
	${MAKE} -C json cov
	${MAKE} -C markdown cov
	${MAKE} -C socket cov
	${MAKE} -C http cov
	${MAKE} -C httpd cov
	${MAKE} -C test test_httpd_cov

test_httpd_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C ekc3 debug
	${MAKE} -C event debug
	${MAKE} -C json debug
	${MAKE} -C markdown debug
	${MAKE} -C socket debug
	${MAKE} -C http debug
	${MAKE} -C httpd debug
	${MAKE} -C test test_httpd_debug

test_ikc3: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C test test_ikc3

test_ikc3_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C test test_ikc3_asan

test_ikc3_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C test test_ikc3_cov

test_ikc3_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C test test_ikc3_debug

test_json: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C json build
	${MAKE} -C test test_json

test_json_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C json asan
	${MAKE} -C test test_json_asan

test_json_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C json cov
	${MAKE} -C test test_json_cov

test_json_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C json debug
	${MAKE} -C test test_json_debug

test_kc3s: kc3s lib_links
	${MAKE} -C test test_kc3s

test_kc3s_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C test test_ekc3_asan

test_kc3s_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C test test_kc3s_cov

test_kc3s_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C test test_kc3s_debug

test_libkc3: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C test test_libkc3

test_libkc3_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C test test_libkc3_cov

test_libkc3_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C test test_libkc3_debug

test_markdown: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C markdown build
	${MAKE} -C test test_markdown

test_markdown_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C markdown asan
	${MAKE} -C test test_markdown_asan

test_markdown_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C markdown cov
	${MAKE} -C test test_markdown_cov

test_markdown_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C markdown debug
	${MAKE} -C test test_markdown_debug

test_socket: lib_links
	${MAKE} -C libtommath build
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 build
	${MAKE} -C ikc3 build
	${MAKE} -C kc3s build
	${MAKE} -C socket build
	${MAKE} -C test test_socket

test_socket_asan: lib_links_asan
	${MAKE} -C libtommath asan
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 asan
	${MAKE} -C ikc3 asan
	${MAKE} -C kc3s asan
	${MAKE} -C socket asan
	${MAKE} -C test test_socket_asan

test_socket_cov: lib_links_cov
	${MAKE} -C libtommath cov
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 cov
	${MAKE} -C ikc3 cov
	${MAKE} -C kc3s cov
	${MAKE} -C socket cov
	${MAKE} -C test test_socket_cov

test_socket_debug: lib_links_debug
	${MAKE} -C libtommath debug
	${MAKE} -C ucd2c
	${MAKE} -C libkc3 debug
	${MAKE} -C ikc3 debug
	${MAKE} -C kc3s debug
	${MAKE} -C socket debug
	${MAKE} -C test test_socket_debug

uninstall:
	rm -rf ${prefix}/lib/kc3
	rm -rf ${prefix}/share/kc3
	${MAKE} -C libtommath uninstall
	${MAKE} -C libkc3 uninstall
	${MAKE} -C ikc3 uninstall
	${MAKE} -C kc3s uninstall
	${MAKE} -C ekc3 uninstall
	${MAKE} -C event uninstall
	${MAKE} -C json uninstall
	${MAKE} -C markdown uninstall
	${MAKE} -C smtp uninstall
	${MAKE} -C socket uninstall
	${MAKE} -C http uninstall
	${MAKE} -C httpd uninstall
	${MAKE} -C window uninstall
	${MAKE} -C gtk4 uninstall
#	${MAKE} -C fs uninstall

.PHONY: all \
	asan \
	assets \
	build \
	dump \
	cov \
	clean \
	clean_cov \
	clean_dump \
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
	fx_assets \
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
	gdb_test_socket \
	gdb_test_socket_asan \
	gdb_test_socket_debug \
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
	kmsg \
	lib_links \
	lib_links_asan \
	lib_links_debug \
	lib_links_bsd \
	lib_links_bsd_asan \
	lib_links_bsd_debug \
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
	test_kc3s_debug
	test_libkc3 \
	test_libkc3_asan \
	test_libkc3_cov \
	test_libkc3_debug \
	test_markdown \
	test_markdown_asan \
	test_markdown_cov \
	test_markdown_debug \
	test_socket \
	test_socket_asan \
	test_socket_cov \
	test_socket_debug \
	uninstall
