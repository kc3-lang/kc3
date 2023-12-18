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
/* Gen from u.h.in BITS=64 bits=64 */
#include "assert.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "u64.h"

u64 * u64_init_cast (u64 *u, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u64) tag->data.character;
    return u;
  case TAG_F32:
    *u = (u64) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (u64) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u64(&tag->data.integer);
    return u;
  case TAG_SW:
    *u = (u64) tag->data.sw;
    return u;
  case TAG_S64:
    *u = (u64) tag->data.s64;
    return u;
  case TAG_S32:
    *u = (u64) tag->data.s32;
    return u;
  case TAG_S16:
    *u = (u64) tag->data.s16;
    return u;
  case TAG_S8:
    *u = (u64) tag->data.s8;
    return u;
  case TAG_U8:
    *u = (u64) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (u64) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (u64) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (u64) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (u64) tag->data.uw;
    return u;
  default:
    break;
  }
  warnx("u64_cast: cannot cast %s to u64",
        tag_type_to_string(tag->type));
  assert(! "u64_cast: cannot cast to u64");
  return NULL;
}

u64 * u64_init_copy (u64 *u, const u64 *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

u64 * u64_random (u64 *u)
{
  arc4random_buf(u, sizeof(u64));
  return u;
}

#if 64 > 32

u64 * u64_random_uniform (u64 *u, u64 max)
{
  u64 rest;
  u64 result;
  assert(u);
  assert(max);
  rest = max;
  result = 0;
  while (rest > (u32) -1) {
    rest >>= 32;
    result <<= 32;
    result += arc4random();
  }
  if (rest) {
    result <<= 32;
    result += arc4random_uniform(rest);
  }
  *u = result;
  return u;
}

#else

u64 * u64_random_uniform (u64 *u, u64 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif
