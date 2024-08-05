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

CLEANFILES = http_request_offsets http_response_offsets print_limits

CPPFLAGS += -I/usr/local/include -I.. -W -Wall -Werror

all: http_request_offsets http_response_offsets print_limits

build: all

clean:
	rm -rf ${CLEANFILES}

distclean: clean

http_request_offsets: http_request_offsets.c
	${CC} ${CPPFLAGS} -I.. http_request_offsets.c -o http_request_offsets

http_response_offsets: http_response_offsets.c
	${CC} ${CPPFLAGS} -I.. http_response_offsets.c -o http_response_offsets

print_limits: print_limits.c
	libtool --mode=link ${CC} ${CPPFLAGS} -I.. print_limits.c -L../libkc3 -lkc3 -o print_limits

.PHONY: \
	all \
	build \
	clean \
	distclean
