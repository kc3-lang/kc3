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
/* Gen from u.h.in BITS=W bits=w */
#include <assert.h>
#include <err.h>
#include "integer.h"
#include "tag.h"
#include "uw.h"

uw * uw_cast (s_tag *tag, uw *dest)
{
  switch (tag->type) {
  case TAG_BOOL:
    *dest = tag->data.bool ? 1 : 0;
    return dest;
  case TAG_CHARACTER:
    *dest = (uw) tag->data.character;
    return dest;
  case TAG_F32:
    *dest = (uw) tag->data.f32;
    return dest;
  case TAG_F64:
    *dest = (uw) tag->data.f64;
    return dest;
  case TAG_INTEGER:
    *dest = integer_to_uw(&tag->data.integer);
    return dest;
  case TAG_SW:
    *dest = (uw) tag->data.sw;
    return dest;
  case TAG_S64:
    *dest = (uw) tag->data.s64;
    return dest;
  case TAG_S32:
    *dest = (uw) tag->data.s32;
    return dest;
  case TAG_S16:
    *dest = (uw) tag->data.s16;
    return dest;
  case TAG_S8:
    *dest = (uw) tag->data.s8;
    return dest;
  case TAG_U8:
    *dest = (uw) tag->data.u8;
    return dest;
  case TAG_U16:
    *dest = (uw) tag->data.u16;
    return dest;
  case TAG_U32:
    *dest = (uw) tag->data.u32;
    return dest;
  case TAG_U64:
    *dest = (uw) tag->data.u64;
    return dest;
  case TAG_UW:
    *dest = (uw) tag->data.uw;
    return dest;
  default:
    break;
  }
  errx(1, "uw_cast: cannot cast %s to uw",
       tag_type_to_string(tag->type));
  return 0;
}

uw * uw_init_copy (uw *dest, const uw *src)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}
