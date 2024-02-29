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
/* Gen from s.h.in BITS=16 bits=16 */
#include "assert.h"
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "ratio.h"
#include "s16.h"

s16 * s16_init_cast (s16 *s, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (s16) tag->data.character;
    return s;
  case TAG_F32:
    *s = (s16) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (s16) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_s16(&tag->data.integer);
    return s;
  case TAG_RATIO:
    *s = ratio_to_s16(&tag->data.ratio);
    return s;
  case TAG_SW:
    *s = (s16) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (s16) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (s16) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (s16) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (s16) tag->data.s8;
    return s;
  case TAG_U8:
    *s = (s16) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (s16) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (s16) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (s16) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (s16) tag->data.uw;
    return s;
  default:
    break;
  }
  err_write_1("s16_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to s16");
  assert(! "s16_cast: cannot cast to s16");
  return NULL;
}

s16 * s16_init_copy (s16 *s, const s16 *src)
{
  assert(src);
  assert(s);
  *s = *src;
  return s;
}

s16 * s16_random (s16 *s)
{
  arc4random_buf(s, sizeof(s16));
  return s;
}
