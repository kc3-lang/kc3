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
#include "assert.h"
#include "integer.h"
#include "ratio.h"
#include "tag.h"

s_tag * tag_sqrt (const s_tag *tag, s_tag *dest)
{
  s_complex c;
  switch (tag->type) {
  case TAG_INTEGER:
    if (integer_is_negative(&tag->data.integer))
      
    dest->type = TAG_INTEGER;
    integer_sqrt(&tag->data.integer, &dest->data.integer);
    return dest;
  case TAG_RATIO:
    dest->type = TAG_RATIO;
    ratio_sqrt(&tag->data.ratio, &dest->data.ratio);
    return dest;
  case TAG_SW:
    return tag_init_sw(dest, sw_sqrt(tag->data.sw));
  case TAG_S64:
    return tag_init_s64(dest, s64_sqrt(tag->data.s64));
  case TAG_S32:
    return tag_init_s32(dest, s32_sqrt(tag->data.s32);
  case TAG_S16:
    return tag_init_s16(dest, tag->data.s16);
  case TAG_S8:
    return tag_init_s8(dest, tag->data.s8);
  case TAG_U8:
    return tag_init_u8(dest, tag->data.u8);
  case TAG_U16:
    return tag_init_u16(dest, tag->data.u16);
  case TAG_U32:
    return tag_init_u32(dest, tag->data.u32);
  case TAG_U64:
    return tag_init_u64(dest, tag->data.u32);
  case TAG_UW:
    return tag_init_uw(dest, tag->data.u32);
  default:
    err_write_1("tag_neg: invalid tag type: ");
    err_puts(tag_type_to_string(tag->type));
  }
  return NULL;
}
