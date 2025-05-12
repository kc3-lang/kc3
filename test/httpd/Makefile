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

SRC_TOP = ../..

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

test: vegeta_local

VEGETA_RATES = 10 20

vegeta_local:
	for RATE in ${VEGETA_RATES}; do \
		vegeta attack -rate=$$RATE -duration=20s < \
			vegeta-targets.local.txt | \
		tee vegeta-result.rate$$RATE.local.bin | \
		vegeta plot > vegeta-result.rate$$RATE.local.html; \
	done

vegeta_production:
	for RATE in ${VEGETA_RATES}; do \
		vegeta attack -rate=$$RATE -duration=20s < \
			vegeta-targets.production.txt | \
		tee vegeta-result.rate$$RATE.production.bin | \
		vegeta plot > vegeta-result.rate$$RATE.production.html; \
	done

.PHONY: assets asan cov debug main run test vegeta_local vegeta_production
