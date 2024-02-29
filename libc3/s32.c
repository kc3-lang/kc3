/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
/* Gen from s.h.in BITS=32 bits=32 */
#include "assert.h"
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "ratio.h"
#include "s32.h"

s32 * s32_init_cast (s32 *s, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (s32) tag->data.character;
    return s;
  case TAG_F32:
    *s = (s32) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (s32) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_s32(&tag->data.integer);
    return s;
  case TAG_RATIO:
    *s = ratio_to_s32(&tag->data.ratio);
    return s;
  case TAG_SW:
    *s = (s32) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (s32) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (s32) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (s32) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (s32) tag->data.s8;
    return s;
  case TAG_U8:
    *s = (s32) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (s32) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (s32) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (s32) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (s32) tag->data.uw;
    return s;
  default:
    break;
  }
  err_write_1("s32_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to s32");
  assert(! "s32_cast: cannot cast to s32");
  return NULL;
}

s32 * s32_init_copy (s32 *s, const s32 *src)
{
  assert(src);
  assert(s);
  *s = *src;
  return s;
}

s32 * s32_random (s32 *s)
{
  arc4random_buf(s, sizeof(s32));
  return s;
}

s_tag * s32_sqrt (const s32 x, s_tag *dest)
{
  assert(dest);
  if (x < 0) {
    // FIXME
    //dest->type = TAG_COMPLEX;
    return NULL;
  }
  dest->type = TAG_S32;
  dest->data.s32 = (s32) sqrtl((long double) x);
  return dest;
}
