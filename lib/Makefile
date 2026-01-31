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

all:
	find . -name '[^.]*.kc3' | while read SRC; do \
		echo "echo '$$SRC' && kc3s --load '$$SRC' --quit"; \
	done | runj 1 /bin/sh

include ../config.mk

clean:
	find . -name '*.kc3c' -print0 | xargs -0 rm

install:
	find kc3 -name '*.kc3c' | cpio -pad ${DESTDIR}${prefix}/lib

.PHONY: all clean
