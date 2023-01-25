## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software excepted
## on Apple computers granted the above copyright notice and
## this permission paragraph are included in all copies and
## substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

CLEANFILES = *.a *.gcno *.la .libs *.lo *.o

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda

CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: libc3.la libc3.a

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: libc3_asan.la libc3_asan.a

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov: libc3_cov.la libc3_cov.a

debug: libc3_debug.la libc3_debug.a

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details libc3.html

GENERATED_FILES = \
	set__fact.c set__fact.h \
	set__tag.c set__tag.h \
	set_cursor__fact.c set_cursor__fact.h \
	set_cursor__tag.c set_cursor__tag.h \
	set_item__fact.c set_item__fact.h \
	set_item__tag.c set_item__tag.h \
	skiplist__fact.c skiplist__fact.h \
	skiplist_node__fact.c skiplist_node__fact.h

CLEANFILES += ${GENERATED_FILES}

gen: ${GENERATED_FILES}

SED_FACT = sed \
	-e 's/_NAME[$$]/fact/g' \
	-e 's/_TYPE[$$]/s_fact/g'

set_cursor__fact.h: set_cursor.h.in Makefile
	${SED_FACT} < set_cursor.h.in > set_cursor__fact.h

set_cursor__fact.c: set_cursor.c.in Makefile
	${SED_FACT} < set_cursor.c.in > set_cursor__fact.c

set__fact.h: set.h.in Makefile
	${SED_FACT} < set.h.in > set__fact.h

set__fact.c: set.c.in Makefile
	${SED_FACT} < set.c.in > set__fact.c

set_item__fact.h: set_item.h.in Makefile
	${SED_FACT} < set_item.h.in > set_item__fact.h

set_item__fact.c: set_item.c.in Makefile
	${SED_FACT} < set_item.c.in > set_item__fact.c

SED_FACT_P = sed \
	-e 's/_NAME[$$]/fact/g' \
	-e 's/_TYPE[$$]/s_fact */g'

skiplist__fact.h: skiplist.h.in Makefile
	${SED_FACT_P} < skiplist.h.in > skiplist__fact.h

skiplist__fact.c: skiplist.c.in Makefile
	${SED_FACT_P} < skiplist.c.in > skiplist__fact.c

skiplist_node__fact.h: skiplist_node.h.in Makefile
	${SED_FACT_P} < skiplist_node.h.in > skiplist_node__fact.h

skiplist_node__fact.c: skiplist_node.c.in Makefile
	${SED_FACT_P} < skiplist_node.c.in > skiplist_node__fact.c

SED_TAG = sed \
	-e 's/_NAME[$$]/tag/g' \
	-e 's/_TYPE[$$]/s_tag/g'

set_cursor__tag.h: set_cursor.h.in Makefile
	${SED_TAG} < set_cursor.h.in > set_cursor__tag.h

set_cursor__tag.c: set_cursor.c.in Makefile
	${SED_TAG} < set_cursor.c.in > set_cursor__tag.c

set_item__tag.h: set_item.h.in Makefile
	${SED_TAG} < set_item.h.in > set_item__tag.h

set_item__tag.c: set_item.c.in Makefile
	${SED_TAG} < set_item.c.in > set_item__tag.c

set__tag.h: set.h.in Makefile
	${SED_TAG} < set.h.in > set__tag.h

set__tag.c: set.c.in Makefile
	${SED_TAG} < set.c.in > set__tag.c

install:
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/include/c3
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${HEADERS} ${prefix}/include/c3
	${INSTALL} -o ${OWNER} -g ${GROUP} -m 0755 -d ${prefix}/lib
	${LIBTOOL} --tag=CC --mode=install ${INSTALL} -o ${OWNER} -g ${GROUP} -m 0644 ${LIB} ${prefix}/lib

libc3.a: libc3.la
	cp .libs/libc3.a ./

libc3_asan.a: libc3_asan.la
	cp .libs/libc3_asan.a ./

libc3_cov.a: libc3_cov.la
	cp .libs/libc3_cov.a ./

libc3_debug.a: libc3_debug.la
	cp .libs/libc3_debug.a ./

test:

update_sources:
	./update_sources

.PHONY: all asan build clean cov debug distclean gen install test update_sources

include config.mk
include sources.mk
