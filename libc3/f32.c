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
#include <assert.h>
#include <err.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "f32.h"
#include "u32.h"

f32 * f32_init_cast (f32 *x, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *x = tag->data.bool ? 1.0f : 0.0f;
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
  warnx("f32_init_cast: cannot cast %s to f32",
        tag_type_to_string(tag->type));
  assert(! "f32_init_cast: cannot cast to f32");
  return NULL;
}

f32 * f32_init_copy (f32 *x, const f32 *src)
{
  assert(src);
  assert(x);
  *x = *src;
  return x;
}

f32 * f32_random (f32 *x)
{
  u32 i;
  const u32 max = (1 << 24) - 1;
  u32_random_uniform(max, &i);
  *x = (f32) i / max;
  return x;
}
