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

s_tag * tag_neg (s_tag *src, s_tag *dest)
{
  s_tag tag = {0};
  s_integer tmp = {0};
  switch (src->type) {
  case TAG_INTEGER:
    tag.type = TAG_INTEGER;
    integer_neg(&src->data.integer, &tag.data.integer);
    goto integer_reduce;
  case TAG_RATIO:
    dest->type = TAG_RATIO;
    ratio_neg(&src->data.ratio, &dest->data.ratio);
    return dest;
  case TAG_SW:
    integer_init_sw(&tmp, src->data.sw);
    tag.type = TAG_INTEGER;
    integer_neg(&tmp, &tag.data.integer);
    integer_clean(&tmp);
    goto integer_reduce;
  case TAG_S64:
    integer_init_s64(&tmp, src->data.s64);
    tag.type = TAG_INTEGER;
    integer_neg(&tmp, &tag.data.integer);
    integer_clean(&tmp);
    goto integer_reduce;
  case TAG_S32:
    tag_init_s64(&tag, - (s64) src->data.s32);
    goto integer_reduce;
  case TAG_S16:
    tag_init_s32(&tag, - (s32) src->data.s16);
    goto integer_reduce;
  case TAG_S8:
    tag_init_s16(&tag, - (s16) src->data.s8);
    goto integer_reduce;
  case TAG_U8:
    tag_init_s16(&tag, - (s16) src->data.u8);
    goto integer_reduce;
  case TAG_U16:
    tag_init_s32(&tag, - (s32) src->data.u16);
    goto integer_reduce;
  case TAG_U32:
    tag_init_s64(&tag, - (s64) src->data.u32);
    goto integer_reduce;
  case TAG_U64:
    integer_init_u64(&tmp, src->data.u64);
    tag.type = TAG_INTEGER;
    integer_neg(&tmp, &tag.data.integer);
    integer_clean(&tmp);
    goto integer_reduce;
  case TAG_UW:
    integer_init_uw(&tmp, src->data.uw);
    tag.type = TAG_INTEGER;
    integer_neg(&tmp, &tag.data.integer);
    integer_clean(&tmp);
    goto integer_reduce;
  default:
    err_write_1("tag_neg: invalid tag type: ");
    err_puts(tag_type_to_string(src->type));
  }
  return NULL;
 integer_reduce:
  if (! tag_integer_reduce(&tag, dest)) {
    tag_clean(&tag);
    return NULL;
  }
  tag_clean(&tag);
  return dest;
}
