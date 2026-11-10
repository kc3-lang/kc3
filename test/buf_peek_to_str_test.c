/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#define buf_clean audit_buf_clean
#define buf_delete audit_buf_delete
#define buf_empty audit_buf_empty
#define buf_f audit_buf_f
#define buf_flush audit_buf_flush
#define buf_ignore audit_buf_ignore
#define buf_ignore_line audit_buf_ignore_line
#define buf_ignore_newline audit_buf_ignore_newline
#define buf_ignore_spaces audit_buf_ignore_spaces
#define buf_ignore_spaces_but_newline audit_buf_ignore_spaces_but_newline
#define buf_init audit_buf_init
#define buf_init_const audit_buf_init_const
#define buf_init_1 audit_buf_init_1
#define buf_init_1_const audit_buf_init_1_const
#define buf_init_1_copy audit_buf_init_1_copy
#define buf_init_alloc audit_buf_init_alloc
#define buf_init_popen audit_buf_init_popen
#define buf_init_str audit_buf_init_str
#define buf_init_str_const audit_buf_init_str_const
#define buf_init_str_copy audit_buf_init_str_copy
#define buf_new audit_buf_new
#define buf_new_alloc audit_buf_new_alloc
#define buf_new_popen audit_buf_new_popen
#define buf_peek_1 audit_buf_peek_1
#define buf_peek_bool audit_buf_peek_bool
#define buf_peek_character_utf8 audit_buf_peek_character_utf8
#define buf_peek_f32 audit_buf_peek_f32
#define buf_peek_f64 audit_buf_peek_f64
#define buf_peek_f80 audit_buf_peek_f80
#define buf_peek_f128 audit_buf_peek_f128
#define buf_peek_next_character_utf8 audit_buf_peek_next_character_utf8
#define buf_peek_s8 audit_buf_peek_s8
#define buf_peek_s16 audit_buf_peek_s16
#define buf_peek_s32 audit_buf_peek_s32
#define buf_peek_s64 audit_buf_peek_s64
#define buf_peek_str audit_buf_peek_str
#define buf_peek_sw audit_buf_peek_sw
#define buf_peek_to_str audit_buf_peek_to_str
#define buf_peek_u8 audit_buf_peek_u8
#define buf_peek_u16 audit_buf_peek_u16
#define buf_peek_u32 audit_buf_peek_u32
#define buf_peek_u64 audit_buf_peek_u64
#define buf_peek_uw audit_buf_peek_uw
#define buf_read audit_buf_read
#define buf_read_1 audit_buf_read_1
#define buf_read_bool audit_buf_read_bool
#define buf_read_character_utf8 audit_buf_read_character_utf8
#define buf_read_f32 audit_buf_read_f32
#define buf_read_f64 audit_buf_read_f64
#define buf_read_f80 audit_buf_read_f80
#define buf_read_f128 audit_buf_read_f128
#define buf_read_integer audit_buf_read_integer
#define buf_read_line audit_buf_read_line
#define buf_read_max audit_buf_read_max
#define buf_read_s8 audit_buf_read_s8
#define buf_read_s16 audit_buf_read_s16
#define buf_read_s32 audit_buf_read_s32
#define buf_read_s64 audit_buf_read_s64
#define buf_read_str audit_buf_read_str
#define buf_read_sw audit_buf_read_sw
#define buf_read_sym audit_buf_read_sym
#define buf_read_to_str audit_buf_read_to_str
#define buf_read_to_sym audit_buf_read_to_sym
#define buf_read_u8 audit_buf_read_u8
#define buf_read_u16 audit_buf_read_u16
#define buf_read_u32 audit_buf_read_u32
#define buf_read_u64 audit_buf_read_u64
#define buf_read_until_1_into_str audit_buf_read_until_1_into_str
#define buf_read_until_character_into_str \
  audit_buf_read_until_character_into_str
#define buf_read_until_list_into_str audit_buf_read_until_list_into_str
#define buf_read_until_space_into_str audit_buf_read_until_space_into_str
#define buf_read_until_str_into_buf audit_buf_read_until_str_into_buf
#define buf_read_until_str_into_file audit_buf_read_until_str_into_file
#define buf_read_until_str_into_str audit_buf_read_until_str_into_str
#define buf_read_uw audit_buf_read_uw
#define buf_refill audit_buf_refill
#define buf_refill_compact audit_buf_refill_compact
#define buf_seek audit_buf_seek
#define buf_seek_set audit_buf_seek_set
#define buf_slice_to_str audit_buf_slice_to_str
#define buf_str_to_hex audit_buf_str_to_hex
#define buf_tell_r audit_buf_tell_r
#define buf_tell_w audit_buf_tell_w
#define buf_to_str audit_buf_to_str
#define buf_total_size audit_buf_total_size
#define buf_u8_to_hex audit_buf_u8_to_hex
#define buf_u8_to_hex_size audit_buf_u8_to_hex_size
#define buf_vf audit_buf_vf
#define buf_write audit_buf_write
#define buf_write_1 audit_buf_write_1
#define buf_write_1_size audit_buf_write_1_size
#define buf_write_bool audit_buf_write_bool
#define buf_write_character_utf8 audit_buf_write_character_utf8
#define buf_write_character_utf8_size audit_buf_write_character_utf8_size
#define buf_write_f32 audit_buf_write_f32
#define buf_write_f64 audit_buf_write_f64
#define buf_write_f80 audit_buf_write_f80
#define buf_write_f128 audit_buf_write_f128
#define buf_write_integer audit_buf_write_integer
#define buf_write_s8 audit_buf_write_s8
#define buf_write_s16 audit_buf_write_s16
#define buf_write_s32 audit_buf_write_s32
#define buf_write_s64 audit_buf_write_s64
#define buf_write_str audit_buf_write_str
#define buf_write_str_size audit_buf_write_str_size
#define buf_write_str_without_indent audit_buf_write_str_without_indent
#define buf_write_str_without_indent_size \
  audit_buf_write_str_without_indent_size
#define buf_write_sw audit_buf_write_sw
#define buf_write_u8 audit_buf_write_u8
#define buf_write_u16 audit_buf_write_u16
#define buf_write_u32 audit_buf_write_u32
#define buf_write_u64 audit_buf_write_u64
#define buf_write_uw audit_buf_write_uw
#define buf_xfer audit_buf_xfer
#define buf_xfer_reverse audit_buf_xfer_reverse
#include "../libkc3/kc3.h"
static bool g_buf_peek_to_str_alloc_fail;

static s_str * buf_peek_to_str_str_init_alloc_copy (s_str *str,
                                                     u32 size,
                                                     const char *p)
{
  if (g_buf_peek_to_str_alloc_fail) {
    g_buf_peek_to_str_alloc_fail = false;
    return NULL;
  }
  return str_init_alloc_copy(str, size, p);
}

#define str_init_alloc_copy buf_peek_to_str_str_init_alloc_copy
#include "../libkc3/buf.c"
#undef str_init_alloc_copy
#include "test.h"

char buf_peek_to_str_alloc_failure_check (void)
{
  s_buf buf;
  const s_str old = STR("old");
  s_str dest = old;
  sw result;
  if (! buf_init_1_const(&buf, "abc"))
    return 1;
  g_buf_peek_to_str_alloc_fail = true;
  result = buf_peek_to_str(&buf, &dest);
  g_buf_peek_to_str_alloc_fail = false;
  buf_clean(&buf);
  if (dest.free.p_pvoid)
    str_clean(&dest);
  return result != -1 || dest.free.p_pvoid != old.free.p_pvoid ||
    dest.ptr.p_pvoid != old.ptr.p_pvoid || dest.size != old.size;
}

TEST_CASE(buf_peek_to_str_alloc_failure)
{
  TEST_EQ(buf_peek_to_str_alloc_failure_check(), 0);
}
TEST_CASE_END(buf_peek_to_str_alloc_failure)

void buf_peek_to_str_test (void)
{
  TEST_CASE_RUN(buf_peek_to_str_alloc_failure);
}
