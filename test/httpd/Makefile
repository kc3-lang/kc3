
include ../config.mk

main: assets
	${SRC_TOP}/httpd/.libs/kc3_httpd -C ${SRC_TOP}/test/httpd -d 127.0.0.1 15004

assets:
	${MAKE} -C assets

asan: assets
	${SRC_TOP}/httpd/.libs/kc3_httpd_asan -C ${SRC_TOP}/test/httpd -d 127.0.0.1 15004

cov: assets
	${SRC_TOP}/httpd/.libs/kc3_httpd_cov -C ${SRC_TOP}/test/httpd -d 127.0.0.1 15004

debug: assets
	${SRC_TOP}/httpd/.libs/kc3_httpd_debug -C ${SRC_TOP}/test/httpd -d 127.0.0.1 15004

run: assets
	${SRC_TOP}/httpd/.libs/kc3_httpd -C ${SRC_TOP}/test/httpd 127.0.0.1 15004

.PHONY: assets asan cov debug main run
