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
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "f128.h"
#include "u64.h"

f128 * f128_init_cast (f128 *x, const s_tag *tag)
{
  assert(tag);
  assert(x);
  switch (tag->type) {
  case TAG_BOOL:
    *x = tag->data.bool ? 1.0 : 0.0;
    return x;
  case TAG_CHARACTER:
    *x = (f128) tag->data.character;
    return x;
  case TAG_F32:
    *x = (f128) tag->data.f32;
    return x;
  case TAG_F64:
    *x = (f128) tag->data.f64;
    return x;
  case TAG_F128:
    *x = tag->data.f128;
    return x;
  case TAG_INTEGER:
    *x = integer_to_f128(&tag->data.integer);
    return x;
  case TAG_SW:
    *x = (f128) tag->data.sw;
    return x;
  case TAG_S64:
    *x = (f128) tag->data.s64;
    return x;
  case TAG_S32:
    *x = (f128) tag->data.s32;
    return x;
  case TAG_S16:
    *x = (f128) tag->data.s16;
    return x;
  case TAG_S8:
    *x = (f128) tag->data.s8;
    return x;
  case TAG_U8:
    *x = (f128) tag->data.u8;
    return x;
  case TAG_U16:
    *x = (f128) tag->data.u16;
    return x;
  case TAG_U32:
    *x = (f128) tag->data.u32;
    return x;
  case TAG_U64:
    *x = (f128) tag->data.u64;
    return x;
  case TAG_UW:
    *x = (f128) tag->data.uw;
    return x;
  default:
    break;
  }
  warnx("f128_init_cast: cannot cast %s to f128",
        tag_type_to_string(tag->type));
  assert(! "f128_init_cast: cannot cast to f128");
  return NULL;
}

f128 * f128_init_copy (f128 *x, const f128 *src)
{
  assert(src);
  assert(x);
  *x = *src;
  return x;
}

typedef union _128 {
  f128 f128;
  s128 s128;
  u128 u128;
  u64 u64[2];
  s64 s64[2];
} u_128;

f128 * f128_random (f128 *x)
{
  u_128 u;
  u_128 mask;
  arc4random_buf(&u, 15);
  mask.u64[0] = 0x7FFFFFFFFFFFFFFF;
  mask.u64[1] = 0xFFFFFFFFFFFFC000;
  u.u128 &= mask.u128;
  *x = u.f128;
  return x;
}