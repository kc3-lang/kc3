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
/* Gen from u.h.in BITS=16 bits=16 */
#include <assert.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "u16.h"

u16 * u16_cast (s_tag *tag, u16 *dest)
{
  switch (tag->type) {
  case TAG_BOOL:
    *dest = tag->data.bool ? 1 : 0;
    return dest;
  case TAG_CHARACTER:
    *dest = (u16) tag->data.character;
    return dest;
  case TAG_F32:
    *dest = (u16) tag->data.f32;
    return dest;
  case TAG_F64:
    *dest = (u16) tag->data.f64;
    return dest;
  case TAG_INTEGER:
    *dest = integer_to_u16(&tag->data.integer);
    return dest;
  case TAG_SW:
    *dest = (u16) tag->data.sw;
    return dest;
  case TAG_S64:
    *dest = (u16) tag->data.s64;
    return dest;
  case TAG_S32:
    *dest = (u16) tag->data.s32;
    return dest;
  case TAG_S16:
    *dest = (u16) tag->data.s16;
    return dest;
  case TAG_S8:
    *dest = (u16) tag->data.s8;
    return dest;
  case TAG_U8:
    *dest = (u16) tag->data.u8;
    return dest;
  case TAG_U16:
    *dest = (u16) tag->data.u16;
    return dest;
  case TAG_U32:
    *dest = (u16) tag->data.u32;
    return dest;
  case TAG_U64:
    *dest = (u16) tag->data.u64;
    return dest;
  case TAG_UW:
    *dest = (u16) tag->data.uw;
    return dest;
  default:
    break;
  }
  warnx("u16_cast: cannot cast %s to u16",
        tag_type_to_string(tag->type));
  return 0;
}

u16 * u16_init_copy (u16 *dest, const u16 *src)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}

u16 * u16_random (u16 *dest)
{
  arc4random_buf(dest, sizeof(u16));
  return dest;
}

u16 * u16_random_uniform (u16 max, u16 *dest)
{
  uw size = (uw) log2(max) / 8;
  u16 rest = (max - ((1 << size) - 1)) >> size;
  u16 result = 0;
  u16 tmp;
  arc4random_buf(&result, size);
  if (rest) {
    tmp = arc4random_uniform(rest);
    result += tmp;
  }
  *dest = result;
  return dest;
}
