## c3
## Copyright 2022 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted
## the above copyright notice and this permission paragraph
## are included in all copies and substantial portions of this
## software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = ucd2c *.lo *.o

DISTCLEANFILES = ${CLEANFILES} config.mk

build: ucd.c

all: build

UCD: UCD.zip
	rm -rf UCD
	mkdir UCD
	cd UCD && unzip ../UCD.zip

UCD.zip:
	ftp "https://www.unicode.org/Public/UCD/latest/ucd/UCD.zip" -o UCD.zip

license.txt:
	ftp "https://www.unicode.org/license.txt" -o license.txt

UCD/UnicodeData.txt: UCD

ucd.c: UCD/UnicodeData.txt ucd2c
	./ucd2c < UCD/UnicodeData.txt > ucd.c

clean:
	rm -f ${CLEANFILES}

distclean:
	rm -rf ${DISTCLEANFILES}

.PHONY: all build clean distclean

include config.mk
