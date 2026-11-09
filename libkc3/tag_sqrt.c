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
#include <math.h>
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
  case TAG_F32:
    return tag_init_f32(dest, sqrtf(tag->data.td_f32));
  case TAG_F64:
    return tag_init_f64(dest, sqrt(tag->data.td_f64));
#if HAVE_F80
  case TAG_F80:
    return tag_init_f80(dest, sqrtl(tag->data.td_f80));
#endif
#if HAVE_F128
  case TAG_F128:
    return tag_init_f128(dest, __builtin_sqrtf128(tag->data.td_f128));
#endif
  case TAG_INTEGER: return integer_sqrt(&tag->data.td_integer, dest);
  case TAG_RATIO:   return ratio_sqrt(&tag->data.td_ratio, dest);
  case TAG_SW:      return sw_sqrt(tag->data.td_sw, dest);
  case TAG_S64:     return s64_sqrt(tag->data.td_s64, dest);
  case TAG_S32:     return s32_sqrt(tag->data.td_s32, dest);
  case TAG_S16:     return s16_sqrt(tag->data.td_s16, dest);
  case TAG_S8:      return s8_sqrt(tag->data.td_s8, dest);
  case TAG_U8:      return u8_sqrt(tag->data.td_u8, dest);
  case TAG_U16:     return u16_sqrt(tag->data.td_u16, dest);
  case TAG_U32:     return u32_sqrt(tag->data.td_u32, dest);
  case TAG_U64:     return u64_sqrt(tag->data.td_u64, dest);
  case TAG_UW:      return uw_sqrt(tag->data.td_uw, dest);
  default:
    break;
  }
  err_write_1("tag_sqrt: invalid tag type: ");
  err_puts(tag_type_to_string(tag->type));
  assert(! "tag_sqrt: invalid tag type");
  return NULL;
}
