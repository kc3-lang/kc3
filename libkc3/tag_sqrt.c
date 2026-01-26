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
#include "ratio.h"
#include "s8.h"
#include "s16.h"
#include "s32.h"
#include "s64.h"
#include "sw.h"
#include "tag.h"
#include "u8.h"
#include "u16.h"
#include "u32.h"
#include "u64.h"
#include "uw.h"

s_tag * tag_sqrt (s_tag *tag, s_tag *dest)
{
  switch (tag->type) {
  case TAG_INTEGER: return integer_sqrt(&tag->data.integer, dest);
  case TAG_RATIO:   return ratio_sqrt(&tag->data.ratio, dest);
  case TAG_SW:      return sw_sqrt(tag->data.sw, dest);
  case TAG_S64:     return s64_sqrt(tag->data.s64, dest);
  case TAG_S32:     return s32_sqrt(tag->data.s32, dest);
  case TAG_S16:     return s16_sqrt(tag->data.s16, dest);
  case TAG_S8:      return s8_sqrt(tag->data.s8, dest);
  case TAG_U8:      return u8_sqrt(tag->data.u8, dest);
  case TAG_U16:     return u16_sqrt(tag->data.u16, dest);
  case TAG_U32:     return u32_sqrt(tag->data.u32, dest);
  case TAG_U64:     return u64_sqrt(tag->data.u64, dest);
  case TAG_UW:      return uw_sqrt(tag->data.uw, dest);
  default:
    break;
  }
  err_write_1("tag_sqrt: invalid tag type: ");
  err_puts(tag_type_to_string(tag->type));
  assert(! "tag_sqrt: invalid tag type");
  return NULL;
}
