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
/* Gen from u.h.in BITS=8 bits=8 */
#include "assert.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "u8.h"

u8 * u8_init_cast (u8 *u, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u8) tag->data.character;
    return u;
  case TAG_F32:
    *u = (u8) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (u8) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u8(&tag->data.integer);
    return u;
  case TAG_SW:
    *u = (u8) tag->data.sw;
    return u;
  case TAG_S64:
    *u = (u8) tag->data.s64;
    return u;
  case TAG_S32:
    *u = (u8) tag->data.s32;
    return u;
  case TAG_S16:
    *u = (u8) tag->data.s16;
    return u;
  case TAG_S8:
    *u = (u8) tag->data.s8;
    return u;
  case TAG_U8:
    *u = (u8) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (u8) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (u8) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (u8) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (u8) tag->data.uw;
    return u;
  default:
    break;
  }
  warnx("u8_cast: cannot cast %s to u8",
        tag_type_to_string(tag->type));
  assert(! "u8_cast: cannot cast to u8");
  return NULL;
}

u8 * u8_init_copy (u8 *u, const u8 *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

u8 * u8_random (u8 *u)
{
  arc4random_buf(u, sizeof(u8));
  return u;
}

#if 8 > 32

u8 * u8_random_uniform (u8 *u, u8 max)
{
  u8 rest;
  u8 result;
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

u8 * u8_random_uniform (u8 *u, u8 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif
