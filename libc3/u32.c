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
/* Gen from u.h.in BITS=32 bits=32 */
#include <assert.h>
#include <err.h>
#include "integer.h"
#include "tag.h"
#include "u32.h"

u32 * u32_cast (s_tag *tag, u32 *dest)
{
  switch (tag->type) {
  case TAG_VOID:
    *dest = 0;
    return dest;
  case TAG_ARRAY:
    goto ko;
  case TAG_BOOL:
    *dest = tag->data.bool ? 1 : 0;
    return dest;
  case TAG_CALL:
    goto ko;
  case TAG_CFN:
    goto ko;
  case TAG_CHARACTER:
    *dest = (u32) tag->data.character;
    return dest;
  case TAG_F32:
    *dest = (u32) tag->data.f32;
    return dest;
  case TAG_F64:
    *dest = (u32) tag->data.f64;
    return dest;
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
    goto ko;
  case TAG_INTEGER:
    *dest = integer_to_u32(&tag->data.integer);
    return dest;
  case TAG_SW:
    *dest = (u32) tag->data.sw;
    return dest;
  case TAG_S64:
    *dest = (u32) tag->data.s64;
    return dest;
  case TAG_S32:
    *dest = (u32) tag->data.s32;
    return dest;
  case TAG_S16:
    *dest = (u32) tag->data.s16;
    return dest;
  case TAG_S8:
    *dest = (u32) tag->data.s8;
    return dest;
  case TAG_U8:
    *dest = (u32) tag->data.u8;
    return dest;
  case TAG_U16:
    *dest = (u32) tag->data.u16;
    return dest;
  case TAG_U32:
    *dest = (u32) tag->data.u32;
    return dest;
  case TAG_U64:
    *dest = (u32) tag->data.u64;
    return dest;
  case TAG_UW:
    *dest = (u32) tag->data.uw;
    return dest;
  case TAG_LIST:
  case TAG_MAP:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto ko;
  }
  assert(! "u32_cast: unknown tag type");
  errx(1, "u32_cast: unknown tag type: %d", tag->type);
  return 0;
 ko:
  warnx("u32_cast: cannot cast %s to u32",
        tag_type_to_sym(tag->type)->str.ptr.ps8);
  return 0;
}

u32 * u32_copy (const u32 *src, u32 *dest)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}
