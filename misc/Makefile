## kc3
## Copyright 2022-2024 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = http_request http_response limits

CPPFLAGS += -I/usr/local/include -I.. -W -Wall -Werror

all: fact_w http_request http_response limits

build: all

clean:
	rm -rf ${CLEANFILES}

distclean: clean

fact_w: fact_w.c
	${CC} ${CPPFLAGS} -I.. fact_w.c -o fact_w

http_request: http_request.c
	${CC} ${CPPFLAGS} -I.. http_request.c -o http_request

http_response: http_response.c
	${CC} ${CPPFLAGS} -I.. http_response.c -o http_response

limits: limits.c
	libtool --mode=link --tag=CC ${CC} ${CPPFLAGS} -I.. limits.c -L../libkc3 -lkc3 -o limits

.PHONY: \
	all \
	build \
	clean \
	distclean
