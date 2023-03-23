## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

GENERATED_FILES = \
	set__fact.c set__fact.h \
	set__tag.c set__tag.h \
	set_cursor__fact.c set_cursor__fact.h \
	set_cursor__tag.c set_cursor__tag.h \
	set_item__fact.c set_item__fact.h \
	set_item__tag.c set_item__tag.h \
	skiplist__fact.c skiplist__fact.h \
	skiplist_node__fact.c skiplist_node__fact.h

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

include sources.mk
