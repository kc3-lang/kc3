/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "tag.h"

s_tag * tag_bnot (s_tag *tag, s_tag *result)
{
  assert(tag);
  assert(result);
  switch (tag->type) {
  case TAG_INTEGER:
    result->type = TAG_INTEGER;
    integer_bnot(&tag->data.td_integer, &result->data.td_integer);
    return result;
  case TAG_SW:
    return tag_init_sw(result, ~tag->data.td_sw);
  case TAG_S64:
    return tag_init_s64(result, ~tag->data.td_s64);
  case TAG_S32:
    return tag_init_s32(result, ~tag->data.td_s32);
  case TAG_S16:
    return tag_init_s16(result, ~tag->data.td_s16);
  case TAG_S8:
    return tag_init_s8(result, ~tag->data.td_s8);
  case TAG_U8:
    return tag_init_u8(result, ~tag->data.td_u8);
  case TAG_U16:
    return tag_init_u16(result, ~tag->data.td_u16);
  case TAG_U32:
    return tag_init_u32(result, ~tag->data.td_u32);
  case TAG_U64:
    return tag_init_u64(result, ~tag->data.td_u64);
  case TAG_UW:
    return tag_init_uw(result, ~tag->data.td_uw);
  default:
    err_write_1("tag_bnot: invalid tag type: ");
    err_puts(tag_type_to_string(tag->type));
  }
  return NULL;
}
