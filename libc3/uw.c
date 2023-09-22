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

uw uw_cast (s_tag *tag)
{
  switch (tag->type) {
  case TAG_VOID:
    return 0;
  case TAG_ARRAY:
    goto ko;
  case TAG_BOOL:
    return tag->data.bool ? 1 : 0;
  case TAG_CALL:
    goto ko;
  case TAG_CFN:
    goto ko;
  case TAG_CHARACTER:
    return (uw) tag->data.character;
  case TAG_F32:
    return (uw) tag->data.f32;
  case TAG_F64:
    return (uw) tag->data.f64;
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
    goto ko;
  case TAG_INTEGER:
    return integer_to_uw(&tag->data.integer);
  case TAG_SW:
    return (uw) tag->data.sw;
  case TAG_S64:
    return (uw) tag->data.s64;
  case TAG_S32:
    return (uw) tag->data.s32;
  case TAG_S16:
    return (uw) tag->data.s16;
  case TAG_S8:
    return (uw) tag->data.s8;
  case TAG_U8:
    return (uw) tag->data.u8;
  case TAG_U16:
    return (uw) tag->data.u16;
  case TAG_U32:
    return (uw) tag->data.u32;
  case TAG_U64:
    return (uw) tag->data.u64;
  case TAG_UW:
    return (uw) tag->data.uw;
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto ko;
  }
  assert(! "uw_cast: unknown tag type");
  errx(1, "uw_cast: unknown tag type: %d", tag->type);
  return 0;
 ko:
  warnx("uw_cast: cannot cast %s to uw",
        tag_type_to_sym(tag->type)->str.ptr.ps8);
  return 0;
}
