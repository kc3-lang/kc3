## c3
## Copyright 2022 kmx.io <contact@kmx.io>
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

CLEANFILES = *.a *.gcno *.la libc3.la .libs *.lo *.o

CLEANFILES_COV = *.css *.gcda *.html .libs/*.gcda
CLEANFILES += ${CLEANFILES_COV}

DISTCLEANFILES = ${CLEANFILES} config.mk

build: libc3.la libc3.a

all:
	${MAKE} build
	if ${HAVE_GCOV}; then ${MAKE} cov; fi
	${MAKE} debug
	if ${HAVE_ASAN}; then ${MAKE} asan; fi

asan: libc3.asan.la libc3.asan.a

clean:
	rm -rf ${CLEANFILES}

clean_cov:
	rm -rf ${CLEANFILES_COV}

cov: libc3.cov.la libc3.cov.a

debug: libc3.debug.la libc3.debug.a

distclean:
	rm -rf ${DISTCLEANFILES}

gcovr:
	gcovr --gcov-executable ${GCOV} --html-details libc3.html

install:
	mkdir -p ${PREFIX}/include/c3
	install -m 644 arg.h ${PREFIX}/include/c3/
	install -m 644 bool.h ${PREFIX}/include/c3/
	install -m 644 buf.h ${PREFIX}/include/c3/
	install -m 644 buf_file.h ${PREFIX}/include/c3/
	install -m 644 buf_inspect.h ${PREFIX}/include/c3/
	install -m 644 buf_parse.h ${PREFIX}/include/c3/
	install -m 644 buf_save.h ${PREFIX}/include/c3/
	install -m 644 c3.h ${PREFIX}/include/c3/
	install -m 644 call.h ${PREFIX}/include/c3/
	install -m 644 character.h ${PREFIX}/include/c3/
	install -m 644 compare.h ${PREFIX}/include/c3/
	install -m 644 debug.h ${PREFIX}/include/c3/
	install -m 644 env.h ${PREFIX}/include/c3/
	install -m 644 eval.h ${PREFIX}/include/c3/
	install -m 644 fact.h ${PREFIX}/include/c3/
	install -m 644 facts.h ${PREFIX}/include/c3/
	install -m 644 fn.h ${PREFIX}/include/c3/
	install -m 644 hash.h ${PREFIX}/include/c3/
	install -m 644 ident.h ${PREFIX}/include/c3/
	install -m 644 integer.h ${PREFIX}/include/c3/
	install -m 644 list.h ${PREFIX}/include/c3/
	install -m 644 quote.h ${PREFIX}/include/c3/
	install -m 644 str.h ${PREFIX}/include/c3/
	install -m 644 tag.h ${PREFIX}/include/c3/
	install -m 644 tuple.h ${PREFIX}/include/c3/
	install -m 644 ucd.h ${PREFIX}/include/c3/
	mkdir -p ${PREFIX}/lib
	libtool install libc3.la ${PREFIX}/lib

libc3.a: libc3.la
	cp .libs/libc3.a ./

libc3.asan.a: libc3.asan.la
	cp .libs/libc3.asan.a ./

libc3.cov.a: libc3.cov.la
	cp .libs/libc3.cov.a ./

libc3.debug.a: libc3.debug.la
	cp .libs/libc3.debug.a ./

SED_FACT = sed -e 's/_NAME[$$]/fact/g' -e 's/_TYPE[$$]/s_fact/g'
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

SED_FACT_P = sed -e 's/_NAME[$$]/fact/g' -e 's/_TYPE[$$]/s_fact */g'
skiplist__fact.h: skiplist.h.in Makefile
	${SED_FACT_P} < skiplist.h.in > skiplist__fact.h
skiplist__fact.c: skiplist.c.in Makefile
	${SED_FACT_P} < skiplist.c.in > skiplist__fact.c
skiplist_node__fact.h: skiplist_node.h.in Makefile
	${SED_FACT_P} < skiplist_node.h.in > skiplist_node__fact.h
skiplist_node__fact.c: skiplist_node.c.in Makefile
	${SED_FACT_P} < skiplist_node.c.in > skiplist_node__fact.c

SED_TAG = sed -e 's/_NAME[$$]/tag/g' -e 's/_TYPE[$$]/s_tag/g'
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

test:

update_sources:
	./update_sources

.PHONY: all asan build clean cov debug distclean install test update_sources

include config.mk
include sources.mk
