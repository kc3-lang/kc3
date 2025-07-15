/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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

#include "types.h"
#include "assert.h"
#include "marshall.h"
#include "marshall_read.h"
#include "buf.h"

#define DEF_MARSHALL_READ(name, type)                                  \
  sw marshall_read_ ## name (s_marshall_read *mr, bool heap,           \
                            type *dest)                                \
  {                                                                    \
    s_buf *buf = {0};                                                  \
    assert(mr);                                                        \
    assert(dest);                                                      \
    buf = heap ? &mr->heap : &mr->buf;                                 \
    return buf_read_ ## type (buf, dest) < 0 ? -1 : 0;                 \
  }

DEF_MARSHALL_READ(f128, f128)
DEF_MARSHALL_READ(f32, f32)
DEF_MARSHALL_READ(f64, f64)

DEF_MARSHALL_READ(s8, s8)
DEF_MARSHALL_READ(s16, s16)
DEF_MARSHALL_READ(s32, s32)
DEF_MARSHALL_READ(s64, s64)
DEF_MARSHALL_READ(sw, sw)

DEF_MARSHALL_READ(u8,  u8)
DEF_MARSHALL_READ(u16, u16)
DEF_MARSHALL_READ(u32, u32)
DEF_MARSHALL_READ(u64, u64)
DEF_MARSHALL_READ(uw, uw)
DEF_MARSHALL_READ(bool, bool)


sw marshall_read_caracter(s_marshall_read *mr, bool heap,
                          character *dest)
{
  s_buf *buf = {0};
  assert(mr);
  assert(dest);
  buf = heap ? &mr->heap : &mr->buf;
  return buf_read_character_utf8(buf, dest) < 0 ? -1 : 0;
}

sw marshall_read_str(s_marshall_read *mr, bool heap, s_str *dest)
{
  s_buf *buf = {0};
  assert(mr);
  assert(dest);
  buf = heap ? &mr->heap : &mr->buf;
  return buf_read_str(buf, dest) < 0 ? -1 : 0;
}

// more complex types :

// DEF_MARSHALL_READ(array, s_array)
// DEF_MARSHALL_READ(call, s_call)
// DEF_MARSHALL_READ(callable, s_callable)
// DEF_MARSHALL_READ(character, character) -> buf_read_character_utf8
// DEF_MARSHALL_READ(cow, s_cow)
// DEF_MARSHALL_READ(complex, s_complex)
// DEF_MARSHALL_READ(do_block, s_do_block)
// DEF_MARSHALL_READ(fact, s_fact)
// DEF_MARSHALL_READ(list, s_list)
// DEF_MARSHALL_READ(ident, s_ident)
// DEF_MARSHALL_READ(integer, s_integer)
// DEF_MARSHALL_READ(map, s_map)
// DEF_MARSHALL_READ(quote, s_quote)
// DEF_MARSHALL_READ(ratio, s_ratio)
// DEF_MARSHALL_READ(struct, s_struct)
// DEF_MARSHALL_READ(struct_type, s_struct_type)
// DEF_MARSHALL_READ(sym, s_sym)
// DEF_MARSHALL_READ(tuple, s_tuple)
// DEF_MARSHALL_READ(time, s_time)
// DEF_MARSHALL_READ(unquote, s_unquote)
// DEF_MARSHALL_READ(var, s_var)
