## kc3
## Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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

main: doc release
	${SRC_TOP}/httpd/kc3_httpd --trace -d 127.0.0.1 15004

assets:
	${MAKE} -C assets

asan: doc release
	${SRC_TOP}/httpd/kc3_httpd_asan --trace -d 127.0.0.1 15004

clean_dump:
	rm -f kc3.dump db/*.bin.facts

cov: doc release
	${SRC_TOP}/httpd/kc3_httpd_cov --trace -d 127.0.0.1 15004

debug: doc release
	${SRC_TOP}/httpd/kc3_httpd_debug --trace -d 127.0.0.1 15004

doc:
	rsync -a --delete ../../doc ./

dump: clean_dump
	${SRC_TOP}/httpd/kc3_httpd --trace -d
	time ${SRC_TOP}/kc3s/kc3s --trace --quit

dump_debug: clean_dump
	${SRC_TOP}/httpd/kc3_httpd_debug --trace -d
	time ${SRC_TOP}/kc3s/kc3s_debug --trace --quit

gdb: doc release
	if [ -f kc3_httpd_debug.core ]; then gdb ${SRC_TOP}/httpd/kc3_httpd_debug kc3_httpd_debug.core; else gdb ${SRC_TOP}/httpd/kc3_httpd_debug; fi

gdb_dump: clean_dump
	gdb ${SRC_TOP}/httpd/kc3_httpd_debug

gdb_dump_restore:
	gdb ${SRC_TOP}/kc3s/kc3s_debug

ikc3:
	${SRC_TOP}/ikc3/ikc3 127.0.0.1 15004

lldb_dump: clean_dump
	lldb ${SRC_TOP}/httpd/kc3_httpd_debug

lldb_dump_restore:
	lldb ${SRC_TOP}/kc3s/kc3s_debug

release:
	if [ -d ../../release ]; then \
		rsync -aP ../../release/* static/release/; \
	fi

run:
	${SRC_TOP}/httpd/kc3_httpd -C ${SRC_TOP}/test/httpd 127.0.0.1 15004

test:
	KC3_HTTPD=${SRC_TOP}/httpd/kc3_httpd ./test/run

test_asan:
	KC3_HTTPD=${SRC_TOP}/httpd/kc3_httpd_asan ./test/run

test_debug:
	KC3_HTTPD=${SRC_TOP}/httpd/kc3_httpd_debug ./test/run

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

.PHONY:	assets \
	asan \
	cov \
	debug \
	doc \
	dump \
	main \
	release \
	run \
	test \
	vegeta_local \
	vegeta_production
