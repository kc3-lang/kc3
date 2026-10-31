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
#ifndef LIBKC3_TAG_DIVMOD_H
#define LIBKC3_TAG_DIVMOD_H

#include "types.h"

/* Unsigned magnitudes include S64_MIN without signed negation overflow. */
static bool tag_divmod_fixed_value (const s_tag *tag, u64 *magnitude,
                                    bool *negative)
{
  s64 value;
  *negative = false;
  switch (tag->type) {
  case TAG_S8: value = tag->data.td_s8; break;
  case TAG_S16: value = tag->data.td_s16; break;
  case TAG_S32: value = tag->data.td_s32; break;
  case TAG_S64: value = tag->data.td_s64; break;
  case TAG_SW: value = tag->data.td_sw; break;
  case TAG_U8: *magnitude = tag->data.td_u8; return true;
  case TAG_U16: *magnitude = tag->data.td_u16; return true;
  case TAG_U32: *magnitude = tag->data.td_u32; return true;
  case TAG_U64: *magnitude = tag->data.td_u64; return true;
  case TAG_UW: *magnitude = tag->data.td_uw; return true;
  default: return false;
  }
  *negative = value < 0;
  *magnitude = *negative ? (u64) 0 - (u64) value : (u64) value;
  return true;
}

#endif /* LIBKC3_TAG_DIVMOD_H */
