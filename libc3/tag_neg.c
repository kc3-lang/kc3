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

s_tag * tag_neg (const s_tag *tag, s_tag *result)
{
  s_integer tmp;
  switch (tag->type) {
  case TAG_INTEGER:
    result->type = TAG_INTEGER;
    integer_neg(&tag->data.integer, &result->data.integer);
    return result;
  case TAG_RATIO:
    result->type = TAG_RATIO;
    ratio_neg(&tag->data.ratio, &result->data.ratio);
    return result;
  case TAG_SW:
    if (tag->data.sw == SW_MIN) {
      integer_init_sw(&tmp, tag->data.sw);
      result->type = TAG_INTEGER;
      integer_neg(&tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    }
    return tag_init_sw(result, -tag->data.sw);
  case TAG_S64:
    if (tag->data.s64 == S64_MIN) {
      integer_init_s64(&tmp, tag->data.s64);
      result->type = TAG_INTEGER;
      integer_neg(&tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    }
    return tag_init_s64(result, -tag->data.s64);
  case TAG_S32:
    return tag_init_s64(result, - (s64) tag->data.s32);
  case TAG_S16:
    return tag_init_s32(result, - (s32) tag->data.s16);
  case TAG_S8:
    return tag_init_s16(result, - (s16) tag->data.s8);
  case TAG_U8:
    return tag_init_s16(result, - (s16) tag->data.u8);
  case TAG_U16:
    return tag_init_s32(result, - (s32) tag->data.u16);
  case TAG_U32:
    return tag_init_s64(result, - (s64) tag->data.u32);
  case TAG_U64:
    integer_init_u64(&tmp, tag->data.u64);
    result->type = TAG_INTEGER;
    integer_neg(&tmp, &result->data.integer);
    integer_clean(&tmp);
    return result;
  case TAG_UW:
    integer_init_uw(&tmp, tag->data.uw);
    result->type = TAG_INTEGER;
    integer_neg(&tmp, &result->data.integer);
    integer_clean(&tmp);
    return result;
  default:
    err_write_1("tag_neg: invalid tag type: ");
    err_puts(tag_type_to_string(tag->type));
  }
  return NULL;
}
