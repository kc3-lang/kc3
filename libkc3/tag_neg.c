/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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

s_tag * tag_neg (s_tag *tag, s_tag *dest)
{
  s_integer tmp = {0};
  switch (tag->type) {
  case TAG_INTEGER:
    dest->type = TAG_INTEGER;
    integer_neg(&tag->data.integer, &dest->data.integer);
    return tag_integer_reduce(dest);
  case TAG_RATIO:
    dest->type = TAG_RATIO;
    ratio_neg(&tag->data.ratio, &dest->data.ratio);
    return dest;
  case TAG_SW:
    integer_init_sw(&tmp, tag->data.sw);
    dest->type = TAG_INTEGER;
    integer_neg(&tmp, &dest->data.integer);
    integer_clean(&tmp);
    return tag_integer_reduce(dest);
  case TAG_S64:
    integer_init_s64(&tmp, tag->data.s64);
    dest->type = TAG_INTEGER;
    integer_neg(&tmp, &dest->data.integer);
    integer_clean(&tmp);
    return tag_integer_reduce(dest);
  case TAG_S32:
    tag_init_s64(dest, - (s64) tag->data.s32);
    return tag_integer_reduce(dest);
  case TAG_S16:
    tag_init_s32(dest, - (s32) tag->data.s16);
    return tag_integer_reduce(dest);
  case TAG_S8:
    tag_init_s16(dest, - (s16) tag->data.s8);
    return tag_integer_reduce(dest);
  case TAG_U8:
    tag_init_s16(dest, - (s16) tag->data.u8);
    return tag_integer_reduce(dest);
  case TAG_U16:
    tag_init_s32(dest, - (s32) tag->data.u16);
    return tag_integer_reduce(dest);
  case TAG_U32:
    tag_init_s64(dest, - (s64) tag->data.u32);
    return tag_integer_reduce(dest);
  case TAG_U64:
    integer_init_u64(&tmp, tag->data.u64);
    dest->type = TAG_INTEGER;
    integer_neg(&tmp, &dest->data.integer);
    integer_clean(&tmp);
    return tag_integer_reduce(dest);
  case TAG_UW:
    integer_init_uw(&tmp, tag->data.uw);
    dest->type = TAG_INTEGER;
    integer_neg(&tmp, &dest->data.integer);
    integer_clean(&tmp);
    return tag_integer_reduce(dest);
  default:
    err_write_1("tag_neg: invalid tag type: ");
    err_puts(tag_type_to_string(tag->type));
  }
  return NULL;
}
