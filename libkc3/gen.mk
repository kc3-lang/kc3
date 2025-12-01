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

GENERATED_FILES = \
	buf_inspect_s8.c buf_inspect_s8.h \
	buf_inspect_s8_binary.c buf_inspect_s8_binary.h \
	buf_inspect_s8_octal.c buf_inspect_s8_octal.h \
	buf_inspect_s8_decimal.c buf_inspect_s8_decimal.h \
	buf_inspect_s8_hexadecimal.c buf_inspect_s8_hexadecimal.h \
	buf_inspect_s16.c buf_inspect_s16.h \
	buf_inspect_s16_binary.c buf_inspect_s16_binary.h \
	buf_inspect_s16_octal.c buf_inspect_s16_octal.h \
	buf_inspect_s16_decimal.c buf_inspect_s16_decimal.h \
	buf_inspect_s16_hexadecimal.c buf_inspect_s16_hexadecimal.h \
	buf_inspect_s32.c buf_inspect_s32.h \
	buf_inspect_s32_binary.c buf_inspect_s32_binary.h \
	buf_inspect_s32_octal.c buf_inspect_s32_octal.h \
	buf_inspect_s32_decimal.c buf_inspect_s32_decimal.h \
	buf_inspect_s32_hexadecimal.c buf_inspect_s32_hexadecimal.h \
	buf_inspect_s64.c buf_inspect_s64.h \
	buf_inspect_s64_binary.c buf_inspect_s64_binary.h \
	buf_inspect_s64_octal.c buf_inspect_s64_octal.h \
	buf_inspect_s64_decimal.c buf_inspect_s64_decimal.h \
	buf_inspect_s64_hexadecimal.c buf_inspect_s64_hexadecimal.h \
	buf_inspect_sw.c buf_inspect_sw.h \
	buf_inspect_sw_binary.c buf_inspect_sw_binary.h \
	buf_inspect_sw_octal.c buf_inspect_sw_octal.h \
	buf_inspect_sw_decimal.c buf_inspect_sw_decimal.h \
	buf_inspect_sw_hexadecimal.c buf_inspect_sw_hexadecimal.h \
	buf_inspect_u8.c buf_inspect_u8.h \
	buf_inspect_u8_binary.c buf_inspect_u8_binary.h \
	buf_inspect_u8_octal.c buf_inspect_u8_octal.h \
	buf_inspect_u8_decimal.c buf_inspect_u8_decimal.h \
	buf_inspect_u8_hexadecimal.c buf_inspect_u8_hexadecimal.h \
	buf_inspect_u16.c buf_inspect_u16.h \
	buf_inspect_u16_binary.c buf_inspect_u16_binary.h \
	buf_inspect_u16_octal.c buf_inspect_u16_octal.h \
	buf_inspect_u16_decimal.c buf_inspect_u16_decimal.h \
	buf_inspect_u16_hexadecimal.c buf_inspect_u16_hexadecimal.h \
	buf_inspect_u32.c buf_inspect_u32.h \
	buf_inspect_u32_binary.c buf_inspect_u32_binary.h \
	buf_inspect_u32_octal.c buf_inspect_u32_octal.h \
	buf_inspect_u32_decimal.c buf_inspect_u32_decimal.h \
	buf_inspect_u32_hexadecimal.c buf_inspect_u32_hexadecimal.h \
	buf_inspect_u64.c buf_inspect_u64.h \
	buf_inspect_u64_binary.c buf_inspect_u64_binary.h \
	buf_inspect_u64_octal.c buf_inspect_u64_octal.h \
	buf_inspect_u64_decimal.c buf_inspect_u64_decimal.h \
	buf_inspect_u64_hexadecimal.c buf_inspect_u64_hexadecimal.h \
	buf_inspect_uw.c buf_inspect_uw.h \
	buf_inspect_uw_binary.c buf_inspect_uw_binary.h \
	buf_inspect_uw_octal.c buf_inspect_uw_octal.h \
	buf_inspect_uw_decimal.c buf_inspect_uw_decimal.h \
	buf_inspect_uw_hexadecimal.c buf_inspect_uw_hexadecimal.h \
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
	list_init.h \
	set__fact.c set__fact.h \
	set__uw.c set__uw.h \
	set__tag.c set__tag.h \
	set_cursor__fact.c set_cursor__fact.h \
	set_cursor__uw.c set_cursor__uw.h \
	set_cursor__tag.c set_cursor__tag.h \
	set_item__fact.c set_item__fact.h \
	set_item__uw.c set_item__uw.h \
	set_item__tag.c set_item__tag.h \
	skiplist__fact.c skiplist__fact.h \
	skiplist_node__fact.c skiplist_node__fact.h \
	s8.c s8.h s16.c s16.h s32.c s32.h s64.c s64.h sw.c sw.h \
	tag_init.h \
	u8.c u8.h u16.c u16.h u32.c u32.h u64.c u64.h uw.c uw.h

CLEANFILES += ${GENERATED_FILES}

gen: ${GENERATED_FILES}

list_init.c list_init.h tag_init.c tag_init.h: tag_init.rb ../license.h
	ruby tag_init.rb
	touch list_init.c list_init.h tag_init.c tag_init.h

SED_BITS_8 = sed \
	-e 's/_BITS[$$]/8/g' \
	-e 's/_bits[$$]/8/g'

SED_BITS_8_BINARY = ${SED_BITS_8} \
	-e 's/_BASE[$$]/binary/g'

SED_BITS_8_OCTAL = ${SED_BITS_8} \
	-e 's/_BASE[$$]/octal/g'

SED_BITS_8_DECIMAL = ${SED_BITS_8} \
	-e 's/_BASE[$$]/decimal/g'

SED_BITS_8_HEXADECIMAL = ${SED_BITS_8} \
	-e 's/_BASE[$$]/hexadecimal/g'

buf_inspect_s8.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_8} < buf_inspect_s.c.in > buf_inspect_s8.c

buf_inspect_s8.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_8} < buf_inspect_s.h.in > buf_inspect_s8.h

buf_inspect_s8_binary.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_8_BINARY} < buf_inspect_s_base.c.in > buf_inspect_s8_binary.c

buf_inspect_s8_binary.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_8_BINARY} < buf_inspect_s_base.h.in > buf_inspect_s8_binary.h

buf_inspect_s8_octal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_8_OCTAL} < buf_inspect_s_base.c.in > buf_inspect_s8_octal.c

buf_inspect_s8_octal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_8_OCTAL} < buf_inspect_s_base.h.in > buf_inspect_s8_octal.h

buf_inspect_s8_decimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_8_DECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s8_decimal.c

buf_inspect_s8_decimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_8_DECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s8_decimal.h

buf_inspect_s8_hexadecimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_8_HEXADECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s8_hexadecimal.c

buf_inspect_s8_hexadecimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_8_HEXADECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s8_hexadecimal.h

buf_inspect_u8.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_8} < buf_inspect_u.c.in > buf_inspect_u8.c

buf_inspect_u8.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_8} < buf_inspect_u.h.in > buf_inspect_u8.h

buf_inspect_u8_binary.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_8_BINARY} < buf_inspect_u_base.c.in > buf_inspect_u8_binary.c

buf_inspect_u8_binary.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_8_BINARY} < buf_inspect_u_base.h.in > buf_inspect_u8_binary.h

buf_inspect_u8_octal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_8_OCTAL} < buf_inspect_u_base.c.in > buf_inspect_u8_octal.c

buf_inspect_u8_octal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_8_OCTAL} < buf_inspect_u_base.h.in > buf_inspect_u8_octal.h

buf_inspect_u8_decimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_8_DECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u8_decimal.c

buf_inspect_u8_decimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_8_DECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u8_decimal.h

buf_inspect_u8_hexadecimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_8_HEXADECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u8_hexadecimal.c

buf_inspect_u8_hexadecimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_8_HEXADECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u8_hexadecimal.h

buf_parse_s8.c: buf_parse_s.c.in gen.mk
	${SED_BITS_8} < buf_parse_s.c.in > buf_parse_s8.c

buf_parse_s8.h: buf_parse_s.h.in gen.mk
	${SED_BITS_8} < buf_parse_s.h.in > buf_parse_s8.h

buf_parse_u8.c: buf_parse_u.c.in gen.mk
	${SED_BITS_8} < buf_parse_u.c.in > buf_parse_u8.c

buf_parse_u8.h: buf_parse_u.h.in gen.mk
	${SED_BITS_8} < buf_parse_u.h.in > buf_parse_u8.h

s8.c: s.c.in gen.mk
	${SED_BITS_8} < s.c.in > s8.c

s8.h: s.h.in gen.mk
	${SED_BITS_8} < s.h.in > s8.h

u8.c: u.c.in gen.mk
	${SED_BITS_8} < u.c.in > u8.c

u8.h: u.h.in gen.mk
	${SED_BITS_8} < u.h.in > u8.h

SED_BITS_16 = sed \
	-e 's/_BITS[$$]/16/g' \
	-e 's/_bits[$$]/16/g'

SED_BITS_16_BINARY = ${SED_BITS_16} \
	-e 's/_BASE[$$]/binary/g'

SED_BITS_16_OCTAL = ${SED_BITS_16} \
	-e 's/_BASE[$$]/octal/g'

SED_BITS_16_DECIMAL = ${SED_BITS_16} \
	-e 's/_BASE[$$]/decimal/g'

SED_BITS_16_HEXADECIMAL = ${SED_BITS_16} \
	-e 's/_BASE[$$]/hexadecimal/g'

buf_inspect_s16.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_16} < buf_inspect_s.c.in > buf_inspect_s16.c

buf_inspect_s16.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_16} < buf_inspect_s.h.in > buf_inspect_s16.h

buf_inspect_s16_binary.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_16_BINARY} < buf_inspect_s_base.c.in > buf_inspect_s16_binary.c

buf_inspect_s16_binary.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_16_BINARY} < buf_inspect_s_base.h.in > buf_inspect_s16_binary.h

buf_inspect_s16_octal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_16_OCTAL} < buf_inspect_s_base.c.in > buf_inspect_s16_octal.c

buf_inspect_s16_octal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_16_OCTAL} < buf_inspect_s_base.h.in > buf_inspect_s16_octal.h

buf_inspect_s16_decimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_16_DECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s16_decimal.c

buf_inspect_s16_decimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_16_DECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s16_decimal.h

buf_inspect_s16_hexadecimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_16_HEXADECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s16_hexadecimal.c

buf_inspect_s16_hexadecimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_16_HEXADECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s16_hexadecimal.h

buf_inspect_u16.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_16} < buf_inspect_u.c.in > buf_inspect_u16.c

buf_inspect_u16.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_16} < buf_inspect_u.h.in > buf_inspect_u16.h

buf_inspect_u16_binary.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_16_BINARY} < buf_inspect_u_base.c.in > buf_inspect_u16_binary.c

buf_inspect_u16_binary.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_16_BINARY} < buf_inspect_u_base.h.in > buf_inspect_u16_binary.h

buf_inspect_u16_octal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_16_OCTAL} < buf_inspect_u_base.c.in > buf_inspect_u16_octal.c

buf_inspect_u16_octal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_16_OCTAL} < buf_inspect_u_base.h.in > buf_inspect_u16_octal.h

buf_inspect_u16_decimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_16_DECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u16_decimal.c

buf_inspect_u16_decimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_16_DECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u16_decimal.h

buf_inspect_u16_hexadecimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_16_HEXADECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u16_hexadecimal.c

buf_inspect_u16_hexadecimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_16_HEXADECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u16_hexadecimal.h

buf_parse_s16.c: buf_parse_s.c.in gen.mk
	${SED_BITS_16} < buf_parse_s.c.in > buf_parse_s16.c

buf_parse_s16.h: buf_parse_s.h.in gen.mk
	${SED_BITS_16} < buf_parse_s.h.in > buf_parse_s16.h

buf_parse_u16.c: buf_parse_u.c.in gen.mk
	${SED_BITS_16} < buf_parse_u.c.in > buf_parse_u16.c

buf_parse_u16.h: buf_parse_u.h.in gen.mk
	${SED_BITS_16} < buf_parse_u.h.in > buf_parse_u16.h

s16.c: s.c.in gen.mk
	${SED_BITS_16} < s.c.in > s16.c

s16.h: s.h.in gen.mk
	${SED_BITS_16} < s.h.in > s16.h

u16.c: u.c.in gen.mk
	${SED_BITS_16} < u.c.in > u16.c

u16.h: u.h.in gen.mk
	${SED_BITS_16} < u.h.in > u16.h

SED_BITS_32 = sed \
	-e 's/_BITS[$$]/32/g' \
	-e 's/_bits[$$]/32/g'

SED_BITS_32_BINARY = ${SED_BITS_32} \
	-e 's/_BASE[$$]/binary/g'

SED_BITS_32_OCTAL = ${SED_BITS_32} \
	-e 's/_BASE[$$]/octal/g'

SED_BITS_32_DECIMAL = ${SED_BITS_32} \
	-e 's/_BASE[$$]/decimal/g'

SED_BITS_32_HEXADECIMAL = ${SED_BITS_32} \
	-e 's/_BASE[$$]/hexadecimal/g'

buf_inspect_s32.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_32} < buf_inspect_s.c.in > buf_inspect_s32.c

buf_inspect_s32.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_32} < buf_inspect_s.h.in > buf_inspect_s32.h

buf_inspect_s32_binary.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_32_BINARY} < buf_inspect_s_base.c.in > buf_inspect_s32_binary.c

buf_inspect_s32_binary.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_32_BINARY} < buf_inspect_s_base.h.in > buf_inspect_s32_binary.h

buf_inspect_s32_octal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_32_OCTAL} < buf_inspect_s_base.c.in > buf_inspect_s32_octal.c

buf_inspect_s32_octal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_32_OCTAL} < buf_inspect_s_base.h.in > buf_inspect_s32_octal.h

buf_inspect_s32_decimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_32_DECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s32_decimal.c

buf_inspect_s32_decimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_32_DECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s32_decimal.h

buf_inspect_s32_hexadecimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_32_HEXADECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s32_hexadecimal.c

buf_inspect_s32_hexadecimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_32_HEXADECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s32_hexadecimal.h

buf_inspect_u32.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_32} < buf_inspect_u.c.in > buf_inspect_u32.c

buf_inspect_u32.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_32} < buf_inspect_u.h.in > buf_inspect_u32.h

buf_inspect_u32_binary.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_32_BINARY} < buf_inspect_u_base.c.in > buf_inspect_u32_binary.c

buf_inspect_u32_binary.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_32_BINARY} < buf_inspect_u_base.h.in > buf_inspect_u32_binary.h

buf_inspect_u32_octal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_32_OCTAL} < buf_inspect_u_base.c.in > buf_inspect_u32_octal.c

buf_inspect_u32_octal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_32_OCTAL} < buf_inspect_u_base.h.in > buf_inspect_u32_octal.h

buf_inspect_u32_decimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_32_DECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u32_decimal.c

buf_inspect_u32_decimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_32_DECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u32_decimal.h

buf_inspect_u32_hexadecimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_32_HEXADECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u32_hexadecimal.c

buf_inspect_u32_hexadecimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_32_HEXADECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u32_hexadecimal.h

buf_parse_s32.c: buf_parse_s.c.in gen.mk
	${SED_BITS_32} < buf_parse_s.c.in > buf_parse_s32.c

buf_parse_s32.h: buf_parse_s.h.in gen.mk
	${SED_BITS_32} < buf_parse_s.h.in > buf_parse_s32.h

buf_parse_u32.c: buf_parse_u.c.in gen.mk
	${SED_BITS_32} < buf_parse_u.c.in > buf_parse_u32.c

buf_parse_u32.h: buf_parse_u.h.in gen.mk
	${SED_BITS_32} < buf_parse_u.h.in > buf_parse_u32.h

s32.c: s.c.in gen.mk
	${SED_BITS_32} < s.c.in > s32.c

s32.h: s.h.in gen.mk
	${SED_BITS_32} < s.h.in > s32.h

u32.c: u.c.in gen.mk
	${SED_BITS_32} < u.c.in > u32.c

u32.h: u.h.in gen.mk
	${SED_BITS_32} < u.h.in > u32.h

SED_BITS_64 = sed \
	-e 's/_BITS[$$]/64/g' \
	-e 's/_bits[$$]/64/g'

SED_BITS_64_BINARY = ${SED_BITS_64} \
	-e 's/_BASE[$$]/binary/g'

SED_BITS_64_OCTAL = ${SED_BITS_64} \
	-e 's/_BASE[$$]/octal/g'

SED_BITS_64_DECIMAL = ${SED_BITS_64} \
	-e 's/_BASE[$$]/decimal/g'

SED_BITS_64_HEXADECIMAL = ${SED_BITS_64} \
	-e 's/_BASE[$$]/hexadecimal/g'

buf_inspect_s64.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_64} < buf_inspect_s.c.in > buf_inspect_s64.c

buf_inspect_s64.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_64} < buf_inspect_s.h.in > buf_inspect_s64.h

buf_inspect_s64_binary.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_64_BINARY} < buf_inspect_s_base.c.in > buf_inspect_s64_binary.c

buf_inspect_s64_binary.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_64_BINARY} < buf_inspect_s_base.h.in > buf_inspect_s64_binary.h

buf_inspect_s64_octal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_64_OCTAL} < buf_inspect_s_base.c.in > buf_inspect_s64_octal.c

buf_inspect_s64_octal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_64_OCTAL} < buf_inspect_s_base.h.in > buf_inspect_s64_octal.h

buf_inspect_s64_decimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_64_DECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s64_decimal.c

buf_inspect_s64_decimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_64_DECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s64_decimal.h

buf_inspect_s64_hexadecimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_64_HEXADECIMAL} < buf_inspect_s_base.c.in > buf_inspect_s64_hexadecimal.c

buf_inspect_s64_hexadecimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_64_HEXADECIMAL} < buf_inspect_s_base.h.in > buf_inspect_s64_hexadecimal.h

buf_inspect_u64.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_64} < buf_inspect_u.c.in > buf_inspect_u64.c

buf_inspect_u64.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_64} < buf_inspect_u.h.in > buf_inspect_u64.h

buf_inspect_u64_binary.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_64_BINARY} < buf_inspect_u_base.c.in > buf_inspect_u64_binary.c

buf_inspect_u64_binary.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_64_BINARY} < buf_inspect_u_base.h.in > buf_inspect_u64_binary.h

buf_inspect_u64_octal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_64_OCTAL} < buf_inspect_u_base.c.in > buf_inspect_u64_octal.c

buf_inspect_u64_octal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_64_OCTAL} < buf_inspect_u_base.h.in > buf_inspect_u64_octal.h

buf_inspect_u64_decimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_64_DECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u64_decimal.c

buf_inspect_u64_decimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_64_DECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u64_decimal.h

buf_inspect_u64_hexadecimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_64_HEXADECIMAL} < buf_inspect_u_base.c.in > buf_inspect_u64_hexadecimal.c

buf_inspect_u64_hexadecimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_64_HEXADECIMAL} < buf_inspect_u_base.h.in > buf_inspect_u64_hexadecimal.h

buf_parse_s64.c: buf_parse_s.c.in gen.mk
	${SED_BITS_64} < buf_parse_s.c.in > buf_parse_s64.c

buf_parse_s64.h: buf_parse_s.h.in gen.mk
	${SED_BITS_64} < buf_parse_s.h.in > buf_parse_s64.h

buf_parse_u64.c: buf_parse_u.c.in gen.mk
	${SED_BITS_64} < buf_parse_u.c.in > buf_parse_u64.c

buf_parse_u64.h: buf_parse_u.h.in gen.mk
	${SED_BITS_64} < buf_parse_u.h.in > buf_parse_u64.h

s64.c: s.c.in gen.mk
	${SED_BITS_64} < s.c.in > s64.c

s64.h: s.h.in gen.mk
	${SED_BITS_64} < s.h.in > s64.h

u64.c: u.c.in gen.mk
	${SED_BITS_64} < u.c.in > u64.c

u64.h: u.h.in gen.mk
	${SED_BITS_64} < u.h.in > u64.h

SED_BITS_W = sed \
	-e 's/_BITS[$$]/W/g' \
	-e 's/_bits[$$]/w/g'

SED_BITS_W_BINARY = ${SED_BITS_W} \
	-e 's/_BASE[$$]/binary/g'

SED_BITS_W_OCTAL = ${SED_BITS_W} \
	-e 's/_BASE[$$]/octal/g'

SED_BITS_W_DECIMAL = ${SED_BITS_W} \
	-e 's/_BASE[$$]/decimal/g'

SED_BITS_W_HEXADECIMAL = ${SED_BITS_W} \
	-e 's/_BASE[$$]/hexadecimal/g'

buf_inspect_sw.c: buf_inspect_s.c.in gen.mk
	${SED_BITS_W} < buf_inspect_s.c.in > buf_inspect_sw.c

buf_inspect_sw.h: buf_inspect_s.h.in gen.mk
	${SED_BITS_W} < buf_inspect_s.h.in > buf_inspect_sw.h

buf_inspect_sw_binary.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_W_BINARY} < buf_inspect_s_base.c.in > buf_inspect_sw_binary.c

buf_inspect_sw_binary.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_W_BINARY} < buf_inspect_s_base.h.in > buf_inspect_sw_binary.h

buf_inspect_sw_octal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_W_OCTAL} < buf_inspect_s_base.c.in > buf_inspect_sw_octal.c

buf_inspect_sw_octal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_W_OCTAL} < buf_inspect_s_base.h.in > buf_inspect_sw_octal.h

buf_inspect_sw_decimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_W_DECIMAL} < buf_inspect_s_base.c.in > buf_inspect_sw_decimal.c

buf_inspect_sw_decimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_W_DECIMAL} < buf_inspect_s_base.h.in > buf_inspect_sw_decimal.h

buf_inspect_sw_hexadecimal.c: buf_inspect_s_base.c.in gen.mk
	${SED_BITS_W_HEXADECIMAL} < buf_inspect_s_base.c.in > buf_inspect_sw_hexadecimal.c

buf_inspect_sw_hexadecimal.h: buf_inspect_s_base.h.in gen.mk
	${SED_BITS_W_HEXADECIMAL} < buf_inspect_s_base.h.in > buf_inspect_sw_hexadecimal.h

buf_inspect_uw.c: buf_inspect_u.c.in gen.mk
	${SED_BITS_W} < buf_inspect_u.c.in > buf_inspect_uw.c

buf_inspect_uw.h: buf_inspect_u.h.in gen.mk
	${SED_BITS_W} < buf_inspect_u.h.in > buf_inspect_uw.h

buf_inspect_uw_binary.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_W_BINARY} < buf_inspect_u_base.c.in > buf_inspect_uw_binary.c

buf_inspect_uw_binary.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_W_BINARY} < buf_inspect_u_base.h.in > buf_inspect_uw_binary.h

buf_inspect_uw_octal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_W_OCTAL} < buf_inspect_u_base.c.in > buf_inspect_uw_octal.c

buf_inspect_uw_octal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_W_OCTAL} < buf_inspect_u_base.h.in > buf_inspect_uw_octal.h

buf_inspect_uw_decimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_W_DECIMAL} < buf_inspect_u_base.c.in > buf_inspect_uw_decimal.c

buf_inspect_uw_decimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_W_DECIMAL} < buf_inspect_u_base.h.in > buf_inspect_uw_decimal.h

buf_inspect_uw_hexadecimal.c: buf_inspect_u_base.c.in gen.mk
	${SED_BITS_W_HEXADECIMAL} < buf_inspect_u_base.c.in > buf_inspect_uw_hexadecimal.c

buf_inspect_uw_hexadecimal.h: buf_inspect_u_base.h.in gen.mk
	${SED_BITS_W_HEXADECIMAL} < buf_inspect_u_base.h.in > buf_inspect_uw_hexadecimal.h

buf_parse_sw.c: buf_parse_s.c.in gen.mk
	${SED_BITS_W} < buf_parse_s.c.in > buf_parse_sw.c

buf_parse_sw.h: buf_parse_s.h.in gen.mk
	${SED_BITS_W} < buf_parse_s.h.in > buf_parse_sw.h

buf_parse_uw.c: buf_parse_u.c.in gen.mk
	${SED_BITS_W} < buf_parse_u.c.in > buf_parse_uw.c

buf_parse_uw.h: buf_parse_u.h.in gen.mk
	${SED_BITS_W} < buf_parse_u.h.in > buf_parse_uw.h

sw.c: s.c.in gen.mk
	${SED_BITS_W} < s.c.in > sw.c

sw.h: s.h.in gen.mk
	${SED_BITS_W} < s.h.in > sw.h

uw.c: u.c.in gen.mk
	${SED_BITS_W} < u.c.in > uw.c

uw.h: u.h.in gen.mk
	${SED_BITS_W} < u.h.in > uw.h

SED_FACT = sed \
	-e 's/_NAME[$$]/fact/g' \
	-e 's/_TYPE[$$]/s_fact */g'

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

SED_ALLOC = sed \
	-e 's/_NAME[$$]/alloc/g' \
	-e 's/_TYPE[$$]/s_alloc */g'

skiplist__alloc.h: skiplist.h.in gen.mk
	${SED_ALLOC} < skiplist.h.in > skiplist__alloc.h

skiplist__alloc.c: skiplist.c.in gen.mk
	${SED_ALLOC} < skiplist.c.in > skiplist__alloc.c

skiplist_node__alloc.h: skiplist_node.h.in gen.mk
	${SED_ALLOC} < skiplist_node.h.in > skiplist_node__alloc.h

skiplist_node__alloc.c: skiplist_node.c.in gen.mk
	${SED_ALLOC} < skiplist_node.c.in > skiplist_node__alloc.c

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

SED_UW = sed \
	-e 's/_NAME[$$]/uw/g' \
	-e 's/_TYPE[$$]/uw/g'

set_cursor__uw.h: set_cursor.h.in gen.mk
	${SED_UW} < set_cursor.h.in > set_cursor__uw.h

set_cursor__uw.c: set_cursor.c.in gen.mk
	${SED_UW} < set_cursor.c.in > set_cursor__uw.c

set_item__uw.h: set_item.h.in gen.mk
	${SED_UW} < set_item.h.in > set_item__uw.h

set_item__uw.c: set_item.c.in gen.mk
	${SED_UW} < set_item.c.in > set_item__uw.c

set__uw.h: set.h.in gen.mk
	${SED_UW} < set.h.in > set__uw.h

set__uw.c: set.c.in gen.mk
	${SED_UW} < set.c.in > set__uw.c

SED_TAG = sed \
	-e 's/_NAME[$$]/tag/g' \
	-e 's/_TYPE[$$]/s_tag */g'

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
