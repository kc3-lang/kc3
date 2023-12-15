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
/* Gen from s.h.in BITS=W bits=w */
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "sw.h"

sw * sw_cast (s_tag *tag, sw *dest)
{
  switch (tag->type) {
  case TAG_BOOL:
    *dest = tag->data.bool ? 1 : 0;
    return dest;
  case TAG_CHARACTER:
    *dest = (sw) tag->data.character;
    return dest;
  case TAG_F32:
    *dest = (sw) tag->data.f32;
    return dest;
  case TAG_F64:
    *dest = (sw) tag->data.f64;
    return dest;
  case TAG_INTEGER:
    *dest = integer_to_sw(&tag->data.integer);
    return dest;
  case TAG_SW:
    *dest = (sw) tag->data.sw;
    return dest;
  case TAG_S64:
    *dest = (sw) tag->data.s64;
    return dest;
  case TAG_S32:
    *dest = (sw) tag->data.s32;
    return dest;
  case TAG_S16:
    *dest = (sw) tag->data.s16;
    return dest;
  case TAG_S8:
    *dest = (sw) tag->data.s8;
    return dest;
  case TAG_U8:
    *dest = (sw) tag->data.u8;
    return dest;
  case TAG_U16:
    *dest = (sw) tag->data.u16;
    return dest;
  case TAG_U32:
    *dest = (sw) tag->data.u32;
    return dest;
  case TAG_U64:
    *dest = (sw) tag->data.u64;
    return dest;
  case TAG_UW:
    *dest = (sw) tag->data.uw;
    return dest;
  default:
    break;
  }
  errx(1, "sw_cast: cannot cast %s to sw",
       tag_type_to_string(tag->type));
  return 0;
}

sw * sw_init_copy (sw *dest, const sw *src)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}

sw * sw_random (sw *dest)
{
  arc4random_buf(dest, sizeof(sw));
  return dest;
}
