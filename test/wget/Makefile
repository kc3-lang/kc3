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

LOCAL_TEST = 127.0.0.1:15004
LOCAL_WWW  = 127.0.0.1:15007

CLEANFILES = \
	127.0.0.1:15004

all:
	echo ${MAKE} local_test | local_www

clean:
	rm -rf ${CLEANFILES}

local_test:
	rm -rf ${LOCAL_TEST}
	wget -kmr "http://${LOCAL_TEST}/"

local_www:
	rm -rf ${LOCAL_WWW}
	wget -kmr "http://${LOCAL_WWW}/"

.PHONY: \
	clean \
	local
