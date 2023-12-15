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
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "f64.h"
#include "u64.h"

f64 * f64_init_cast (f64 *x, const s_tag *tag)
{
  assert(tag);
  assert(x);
  switch (tag->type) {
  case TAG_BOOL:
    *x = tag->data.bool ? 1.0 : 0.0;
    return x;
  case TAG_CHARACTER:
    *x = (f64) tag->data.character;
    return x;
  case TAG_F32:
    *x = (f64) tag->data.f32;
    return x;
  case TAG_F64:
    *x = tag->data.f64;
    return x;
  case TAG_INTEGER:
    *x = integer_to_f64(&tag->data.integer);
    return x;
  case TAG_SW:
    *x = (f64) tag->data.sw;
    return x;
  case TAG_S64:
    *x = (f64) tag->data.s64;
    return x;
  case TAG_S32:
    *x = (f64) tag->data.s32;
    return x;
  case TAG_S16:
    *x = (f64) tag->data.s16;
    return x;
  case TAG_S8:
    *x = (f64) tag->data.s8;
    return x;
  case TAG_U8:
    *x = (f64) tag->data.u8;
    return x;
  case TAG_U16:
    *x = (f64) tag->data.u16;
    return x;
  case TAG_U32:
    *x = (f64) tag->data.u32;
    return x;
  case TAG_U64:
    *x = (f64) tag->data.u64;
    return x;
  case TAG_UW:
    *x = (f64) tag->data.uw;
    return x;
  default:
    break;
  }
  warnx("f64_init_cast: cannot cast %s to f64",
        tag_type_to_string(tag->type));
  assert(! "f64_init_cast: cannot cast to f64");
  return NULL;
}

f64 * f64_init_copy (f64 *x, const f64 *src)
{
  assert(src);
  assert(x);
  *x = *src;
  return x;
}

f64 * f64_random (f64 *x)
{
  u64 i;
  const u64 max = ((u64) 1 << 53) - 1;
  u64_random_uniform(max, &i);
  *x = (f64) i / max;
  return x;
}
