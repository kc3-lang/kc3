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
#include <stdlib.h>
#include "assert.h"
#include "f64.h"
#include "integer.h"
#include "ratio.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"
#include "u64.h"

f64 * f64_init_cast (f64 *x, p_sym *type, const s_tag *tag)
{
  assert(x);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_BOOL:
    *x = tag->data.td_bool_ ? 1.0 : 0.0;
    return x;
  case TAG_CHARACTER:
    *x = (f64) tag->data.td_character;
    return x;
  case TAG_F32:
    *x = (f64) tag->data.td_f32;
    return x;
  case TAG_F64:
    *x = tag->data.td_f64;
    return x;
#if HAVE_F80
  case TAG_F80:
    *x = tag->data.td_f80;
    return x;
#endif
#if HAVE_F128
  case TAG_F128:
    *x = tag->data.td_f128;
    return x;
#endif
  case TAG_INTEGER:
    *x = integer_to_f64(&tag->data.td_integer);
    return x;
  case TAG_SW:
    *x = (f64) tag->data.td_sw;
    return x;
  case TAG_S64:
    *x = (f64) tag->data.td_s64;
    return x;
  case TAG_S32:
    *x = (f64) tag->data.td_s32;
    return x;
  case TAG_S16:
    *x = (f64) tag->data.td_s16;
    return x;
  case TAG_S8:
    *x = (f64) tag->data.td_s8;
    return x;
  case TAG_U8:
    *x = (f64) tag->data.td_u8;
    return x;
  case TAG_U16:
    *x = (f64) tag->data.td_u16;
    return x;
  case TAG_U32:
    *x = (f64) tag->data.td_u32;
    return x;
  case TAG_U64:
    *x = (f64) tag->data.td_u64;
    return x;
  case TAG_UW:
    *x = (f64) tag->data.td_uw;
    return x;
  default:
    break;
  }
  err_write_1("f64_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_F64)
    err_puts(" to F64");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka F64");
  }
  assert(! "f64_init_cast: cannot cast to F64");
  return NULL;
}

f64 * f64_init_copy (f64 *x, f64 src)
{
  assert(x);
  *x = src;
  return x;
}

f64 * f64_random (f64 *x)
{
  u64 i;
  const u64 max = ((u64) 1 << 53) - 1;
  u64_random(&i);
  i &= max;
  *x = (f64) i / max;
  return x;
}
