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
#include <assert.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "u8.h"

u8 * u8_cast (s_tag *tag, u8 *dest)
{
  switch (tag->type) {
  case TAG_BOOL:
    *dest = tag->data.bool ? 1 : 0;
    return dest;
  case TAG_CHARACTER:
    *dest = (u8) tag->data.character;
    return dest;
  case TAG_F32:
    *dest = (u8) tag->data.f32;
    return dest;
  case TAG_F64:
    *dest = (u8) tag->data.f64;
    return dest;
  case TAG_INTEGER:
    *dest = integer_to_u8(&tag->data.integer);
    return dest;
  case TAG_SW:
    *dest = (u8) tag->data.sw;
    return dest;
  case TAG_S64:
    *dest = (u8) tag->data.s64;
    return dest;
  case TAG_S32:
    *dest = (u8) tag->data.s32;
    return dest;
  case TAG_S16:
    *dest = (u8) tag->data.s16;
    return dest;
  case TAG_S8:
    *dest = (u8) tag->data.s8;
    return dest;
  case TAG_U8:
    *dest = (u8) tag->data.u8;
    return dest;
  case TAG_U16:
    *dest = (u8) tag->data.u16;
    return dest;
  case TAG_U32:
    *dest = (u8) tag->data.u32;
    return dest;
  case TAG_U64:
    *dest = (u8) tag->data.u64;
    return dest;
  case TAG_UW:
    *dest = (u8) tag->data.uw;
    return dest;
  default:
    break;
  }
  warnx("u8_cast: cannot cast %s to u8",
        tag_type_to_string(tag->type));
  return 0;
}

u8 * u8_init_copy (u8 *dest, const u8 *src)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}

u8 * u8_random (u8 *dest)
{
  arc4random_buf(dest, sizeof(u8));
  return dest;
}

u8 * u8_random_uniform (u8 max, u8 *dest)
{
  uw size = (uw) log2(max) / 8;
  u8 rest = (max - ((1 << size) - 1)) >> size;
  u8 result = 0;
  u8 tmp;
  arc4random_buf(&result, size);
  if (rest) {
    tmp = arc4random_uniform(rest);
    result += tmp;
  }
  *dest = result;
  return dest;
}
