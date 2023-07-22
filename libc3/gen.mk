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
	buf_inspect_s8.c buf_inspect_s8.h \
	buf_inspect_s16.c buf_inspect_s16.h \
	buf_inspect_s32.c buf_inspect_s32.h \
	buf_inspect_s64.c buf_inspect_s64.h \
	buf_inspect_sw.c buf_inspect_sw.h \
	buf_inspect_u8.c buf_inspect_u8.h \
	buf_inspect_u16.c buf_inspect_u16.h \
	buf_inspect_u32.c buf_inspect_u32.h \
	buf_inspect_u64.c buf_inspect_u64.h \
	buf_inspect_u64_hex.c buf_inspect_u64_hex.h \
	buf_inspect_uw.c buf_inspect_uw.h \
	buf_parse_s8.c buf_parse_s8.h \
	buf_parse_s16.c buf_parse_s16.h \
	buf_parse_s32.c buf_parse_s32.h \
	buf_parse_s64.c buf_parse_s64.h \
	buf_parse_sw.c buf_parse_sw.h \
	buf_parse_u8.c buf_parse_u8.h \
	buf_parse_u16.c buf_parse_u16.h \
	buf_parse_u32.c buf_parse_u32.h \
	buf_parse_u64.c buf_parse_u64.h \
	buf_parse_uw.c buf_parse_uw.h \
	set__fact.c set__fact.h \
	set__tag.c set__tag.h \
	set_cursor__fact.c set_cursor__fact.h \
	set_cursor__tag.c set_cursor__tag.h \
	set_item__fact.c set_item__fact.h \
	set_item__tag.c set_item__tag.h \
	skiplist__fact.c skiplist__fact.h \
	skiplist_node__fact.c skiplist_node__fact.h

gen: ${GENERATED_FILES}

SED_BITS_8 = sed \
	-e 's/_BITS[$$]/8/g' \
	-e 's/_bits[$$]/8/g'

buf_inspect_s8.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_8} < buf_inspect_s.c.in > buf_inspect_s8.c

buf_inspect_s8.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_8} < buf_inspect_s.h.in > buf_inspect_s8.h

buf_inspect_u8.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_8} < buf_inspect_u.c.in > buf_inspect_u8.c

buf_inspect_u8.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_8} < buf_inspect_u.h.in > buf_inspect_u8.h

buf_parse_s8.c: buf_parse_s.c.in gen.mk
	${SED_BITS_8} < buf_parse_s.c.in > buf_parse_s8.c

buf_parse_s8.h: buf_parse_s.h.in gen.mk
	${SED_BITS_8} < buf_parse_s.h.in > buf_parse_s8.h

buf_parse_u8.c: buf_parse_u.c.in gen.mk
	${SED_BITS_8} < buf_parse_u.c.in > buf_parse_u8.c

buf_parse_u8.h: buf_parse_u.h.in gen.mk
	${SED_BITS_8} < buf_parse_u.h.in > buf_parse_u8.h

SED_BITS_16 = sed \
	-e 's/_BITS[$$]/16/g' \
	-e 's/_bits[$$]/16/g'

buf_inspect_s16.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_16} < buf_inspect_s.c.in > buf_inspect_s16.c

buf_inspect_s16.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_16} < buf_inspect_s.h.in > buf_inspect_s16.h

buf_inspect_u16.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_16} < buf_inspect_u.c.in > buf_inspect_u16.c

buf_inspect_u16.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_16} < buf_inspect_u.h.in > buf_inspect_u16.h

buf_parse_s16.c: buf_parse_s.c.in gen.mk
	${SED_BITS_16} < buf_parse_s.c.in > buf_parse_s16.c

buf_parse_s16.h: buf_parse_s.h.in gen.mk
	${SED_BITS_16} < buf_parse_s.h.in > buf_parse_s16.h

buf_parse_u16.c: buf_parse_u.c.in gen.mk
	${SED_BITS_16} < buf_parse_u.c.in > buf_parse_u16.c

buf_parse_u16.h: buf_parse_u.h.in gen.mk
	${SED_BITS_16} < buf_parse_u.h.in > buf_parse_u16.h

SED_BITS_32 = sed \
	-e 's/_BITS[$$]/32/g' \
	-e 's/_bits[$$]/32/g'

buf_inspect_s32.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_32} < buf_inspect_s.c.in > buf_inspect_s32.c

buf_inspect_s32.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_32} < buf_inspect_s.h.in > buf_inspect_s32.h

buf_inspect_u32.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_32} < buf_inspect_u.c.in > buf_inspect_u32.c

buf_inspect_u32.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_32} < buf_inspect_u.h.in > buf_inspect_u32.h

buf_parse_s32.c: buf_parse_s.c.in gen.mk
	${SED_BITS_32} < buf_parse_s.c.in > buf_parse_s32.c

buf_parse_s32.h: buf_parse_s.h.in gen.mk
	${SED_BITS_32} < buf_parse_s.h.in > buf_parse_s32.h

buf_parse_u32.c: buf_parse_u.c.in gen.mk
	${SED_BITS_32} < buf_parse_u.c.in > buf_parse_u32.c

buf_parse_u32.h: buf_parse_u.h.in gen.mk
	${SED_BITS_32} < buf_parse_u.h.in > buf_parse_u32.h

SED_BITS_64 = sed \
	-e 's/_BITS[$$]/64/g' \
	-e 's/_bits[$$]/64/g'

SED_BITS_64_HEX = ${SED_BITS_64} \
	-e 's/_BASE[$$]/hexadecimal/g'

buf_inspect_s64.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_64} < buf_inspect_s.c.in > buf_inspect_s64.c

buf_inspect_s64.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_64} < buf_inspect_s.h.in > buf_inspect_s64.h

buf_inspect_u64.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_64} < buf_inspect_u.c.in > buf_inspect_u64.c

buf_inspect_u64.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_64} < buf_inspect_u.h.in > buf_inspect_u64.h

buf_inspect_u64_hex.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_64_HEX} < buf_inspect_u_base.c.in > buf_inspect_u64_hex.c

buf_inspect_u64_hex.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_64} < buf_inspect_u_base.h.in > buf_inspect_u64_hex.h

buf_parse_s64.c: buf_parse_s.c.in gen.mk
	${SED_BITS_64} < buf_parse_s.c.in > buf_parse_s64.c

buf_parse_s64.h: buf_parse_s.h.in gen.mk
	${SED_BITS_64} < buf_parse_s.h.in > buf_parse_s64.h

buf_parse_u64.c: buf_parse_u.c.in gen.mk
	${SED_BITS_64} < buf_parse_u.c.in > buf_parse_u64.c

buf_parse_u64.h: buf_parse_u.h.in gen.mk
	${SED_BITS_64} < buf_parse_u.h.in > buf_parse_u64.h

SED_BITS_W = sed \
	-e 's/_BITS[$$]/W/g' \
	-e 's/_bits[$$]/w/g'

buf_inspect_sw.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_W} < buf_inspect_s.c.in > buf_inspect_sw.c

buf_inspect_sw.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_W} < buf_inspect_s.h.in > buf_inspect_sw.h

buf_inspect_uw.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_W} < buf_inspect_u.c.in > buf_inspect_uw.c

buf_inspect_uw.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_W} < buf_inspect_u.h.in > buf_inspect_uw.h

buf_parse_sw.c: buf_parse_s.c.in gen.mk
	${SED_BITS_W} < buf_parse_s.c.in > buf_parse_sw.c

buf_parse_sw.h: buf_parse_s.h.in gen.mk
	${SED_BITS_W} < buf_parse_s.h.in > buf_parse_sw.h

buf_parse_uw.c: buf_parse_u.c.in gen.mk
	${SED_BITS_W} < buf_parse_u.c.in > buf_parse_uw.c

buf_parse_uw.h: buf_parse_u.h.in gen.mk
	${SED_BITS_W} < buf_parse_u.h.in > buf_parse_uw.h

SED_FACT = sed \
	-e 's/_NAME[$$]/fact/g' \
	-e 's/_TYPE[$$]/s_fact/g'

set_cursor__fact.h: set_cursor.h.in gen.mk
	${SED_FACT} < set_cursor.h.in > set_cursor__fact.h

set_cursor__fact.c: set_cursor.c.in gen.mk
	${SED_FACT} < set_cursor.c.in > set_cursor__fact.c

set__fact.h: set.h.in gen.mk
	${SED_FACT} < set.h.in > set__fact.h

set__fact.c: set.c.in gen.mk
	${SED_FACT} < set.c.in > set__fact.c

set_item__fact.h: set_item.h.in gen.mk
	${SED_FACT} < set_item.h.in > set_item__fact.h

set_item__fact.c: set_item.c.in gen.mk
	${SED_FACT} < set_item.c.in > set_item__fact.c

SED_FACT_P = sed \
	-e 's/_NAME[$$]/fact/g' \
	-e 's/_TYPE[$$]/s_fact */g'

skiplist__fact.h: skiplist.h.in gen.mk
	${SED_FACT_P} < skiplist.h.in > skiplist__fact.h

skiplist__fact.c: skiplist.c.in gen.mk
	${SED_FACT_P} < skiplist.c.in > skiplist__fact.c

skiplist_node__fact.h: skiplist_node.h.in gen.mk
	${SED_FACT_P} < skiplist_node.h.in > skiplist_node__fact.h

skiplist_node__fact.c: skiplist_node.c.in gen.mk
	${SED_FACT_P} < skiplist_node.c.in > skiplist_node__fact.c

SED_TAG = sed \
	-e 's/_NAME[$$]/tag/g' \
	-e 's/_TYPE[$$]/s_tag/g'

set_cursor__tag.h: set_cursor.h.in gen.mk
	${SED_TAG} < set_cursor.h.in > set_cursor__tag.h

set_cursor__tag.c: set_cursor.c.in gen.mk
	${SED_TAG} < set_cursor.c.in > set_cursor__tag.c

set_item__tag.h: set_item.h.in gen.mk
	${SED_TAG} < set_item.h.in > set_item__tag.h

set_item__tag.c: set_item.c.in gen.mk
	${SED_TAG} < set_item.c.in > set_item__tag.c

set__tag.h: set.h.in gen.mk
	${SED_TAG} < set.h.in > set__tag.h

set__tag.c: set.c.in gen.mk
	${SED_TAG} < set.c.in > set__tag.c

include sources.mk
