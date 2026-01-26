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
#include "f32.h"
#include "integer.h"
#include "ratio.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"
#include "u32.h"

f32 f32_clamp (f32 x, f32 min, f32 max)
{
  return ((x < min) ? min : ((x > max) ? max : x));
}

f32 * f32_init_cast (f32 *x, p_sym *type, const s_tag *tag)
{
  assert(x);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_BOOL:
    *x = tag->data.bool_ ? 1.0f : 0.0f;
    return x;
  case TAG_CHARACTER:
    *x = (f32) tag->data.character;
    return x;
  case TAG_F32:
    *x = tag->data.f32;
    return x;
  case TAG_F64:
    *x = (f32) tag->data.f64;
    return x;
  case TAG_INTEGER:
    *x = integer_to_f32(&tag->data.integer);
    return x;
  case TAG_RATIO:
    *x = ratio_to_f32(&tag->data.ratio);
    return x;
  case TAG_SW:
    *x = (f32) tag->data.sw;
    return x;
  case TAG_S64:
    *x = (f32) tag->data.s64;
    return x;
  case TAG_S32:
    *x = (f32) tag->data.s32;
    return x;
  case TAG_S16:
    *x = (f32) tag->data.s16;
    return x;
  case TAG_S8:
    *x = (f32) tag->data.s8;
    return x;
  case TAG_U8:
    *x = (f32) tag->data.u8;
    return x;
  case TAG_U16:
    *x = (f32) tag->data.u16;
    return x;
  case TAG_U32:
    *x = (f32) tag->data.u32;
    return x;
  case TAG_U64:
    *x = (f32) tag->data.u64;
    return x;
  case TAG_UW:
    *x = (f32) tag->data.uw;
    return x;
  default:
    break;
  }
  err_write_1("f32_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_F32)
    err_puts(" to F32");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka F32");
  }
  assert(! "f32_init_cast: cannot cast to F32");
  return NULL;
}

f32 * f32_init_copy (f32 *x, f32 src)
{
  assert(x);
  *x = src;
  return x;
}

f32 f32_random (void)
{
  u32 i;
  const u32 max = ((u32) 1 << 24) - 1;
  u32_random(&i);
  i &= max;
  return (f32) i / max;
}
