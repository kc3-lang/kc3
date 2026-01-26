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

LOCAL_KC3 = 127.0.0.1:15004
LOCAL_WWW  = 127.0.0.1:15007

DT_KC3 = dt.kc3-lang.org
DT_WWW = www-dt.kmx.io

PROD_KC3 = kc3-lang.org
PROD_WWW = www.kmx.io

CLEANFILES = \
	${LOCAL_KC3} \
	${LOCAL_WWW} \
	${DT_KC3} \
	${DT_WWW} \
	${PROD_KC3} \
	${PROD_WWW} \

all:
	echo "${MAKE} local_kc3 | local_www"

dt_kc3:
	rm -rf ${DT_KC3}
	wget -qkmr "http://${DT_KC3}/"

dt_www:
	rm -rf ${DT_WWW}
	wget -qkmr "http://${DT_WWW}/"

local_kc3:
	rm -rf ${LOCAL_KC3}
	wget -qkmr "http://${LOCAL_KC3}/"

local_www:
	rm -rf ${LOCAL_WWW}
	wget -qkmr "http://${LOCAL_WWW}/"

prod_kc3:
	rm -rf ${PROD_KC3}
	wget -qkmr "http://${PROD_KC3}/"

prod_www:
	rm -rf ${PROD_WWW}
	wget -qkmr "http://${PROD_WWW}/"

.PHONY: \
	dt_kc3 \
	dt_www \
	local_kc3 \
	local_www \
	prod_kc3 \
	prod_www
