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
/* Gen from s.h.in BITS=8 bits=8 */
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "s8.h"

s8 * s8_init_cast (s8 *s, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (s8) tag->data.character;
    return s;
  case TAG_F32:
    *s = (s8) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (s8) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_s8(&tag->data.integer);
    return s;
  case TAG_SW:
    *s = (s8) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (s8) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (s8) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (s8) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (s8) tag->data.s8;
    return s;
  case TAG_U8:
    *s = (s8) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (s8) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (s8) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (s8) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (s8) tag->data.uw;
    return s;
  default:
    break;
  }
  warnx("s8_cast: cannot cast %s to s8",
        tag_type_to_string(tag->type));
  assert(! "s8_cast: cannot cast to s8");
  return NULL;
}

s8 * s8_init_copy (s8 *s, const s8 *src)
{
  assert(src);
  assert(s);
  *s = *src;
  return s;
}

s8 * s8_random (s8 *s)
{
  arc4random_buf(s, sizeof(s8));
  return s;
}
