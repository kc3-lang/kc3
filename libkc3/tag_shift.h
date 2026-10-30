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
#ifndef LIBKC3_TAG_SHIFT_H
#define LIBKC3_TAG_SHIFT_H

#include "assert.h"
#include "integer.h"
#include "tag.h"

/* Saturate counts beyond U64: all supported widths and backend bit
   indices are smaller, so their exact value cannot change the result. */
static bool tag_shift_count (const s_tag *tag, u64 *dest)
{
  s64 count;
  switch (tag->type) {
  case TAG_BOOL: *dest = tag->data.td_bool_ ? 1 : 0; return true;
  case TAG_CHARACTER: *dest = tag->data.td_character; return true;
  case TAG_U8: *dest = tag->data.td_u8; return true;
  case TAG_U16: *dest = tag->data.td_u16; return true;
  case TAG_U32: *dest = tag->data.td_u32; return true;
  case TAG_U64: *dest = tag->data.td_u64; return true;
  case TAG_UW: *dest = tag->data.td_uw; return true;
  case TAG_S8: count = tag->data.td_s8; break;
  case TAG_S16: count = tag->data.td_s16; break;
  case TAG_S32: count = tag->data.td_s32; break;
  case TAG_S64: count = tag->data.td_s64; break;
  case TAG_SW: count = tag->data.td_sw; break;
  case TAG_INTEGER:
    if (integer_is_negative(&tag->data.td_integer))
      return false;
    if (tag->data.td_integer.in_int.used >
        (64 + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT ||
        mp_count_bits(&tag->data.td_integer.in_int) > 64)
      *dest = U64_MAX;
    else
      *dest = integer_to_u64(&tag->data.td_integer);
    return true;
  default:
    return false;
  }
  if (count < 0)
    return false;
  *dest = (u64) count;
  return true;
}

/* Reconstruct signed results using only representable conversions. */
static s64 tag_shift_signed (u64 bits, u64 mask)
{
  if (bits > (mask >> 1))
    return -1 - (s64) (mask - bits);
  return (s64) bits;
}

static s_tag * tag_shift_fixed (const s_tag *a, u64 count, bool right,
                                s_tag *dest)
{
  u64 bits;
  u64 mask;
  bool negative = false;
  s_tag tmp = {0};
  uw width;
#define TAG_SHIFT_SIGNED(type, field)                                 \
  case TAG_ ## type:                                                   \
    bits = (u64) a->data.field;                                        \
    negative = a->data.field < 0;                                      \
    width = 8 * sizeof(a->data.field);                                 \
    break
#define TAG_SHIFT_UNSIGNED(type, field)                               \
  case TAG_ ## type:                                                   \
    bits = a->data.field;                                              \
    width = 8 * sizeof(a->data.field);                                 \
    break
  switch (a->type) {
  case TAG_BOOL:
    bits = a->data.td_bool_ ? 1 : 0;
    width = 8;
    break;
  TAG_SHIFT_UNSIGNED(CHARACTER, td_character);
  TAG_SHIFT_SIGNED(S8, td_s8);
  TAG_SHIFT_SIGNED(S16, td_s16);
  TAG_SHIFT_SIGNED(S32, td_s32);
  TAG_SHIFT_SIGNED(S64, td_s64);
  TAG_SHIFT_SIGNED(SW, td_sw);
  TAG_SHIFT_UNSIGNED(U8, td_u8);
  TAG_SHIFT_UNSIGNED(U16, td_u16);
  TAG_SHIFT_UNSIGNED(U32, td_u32);
  TAG_SHIFT_UNSIGNED(U64, td_u64);
  TAG_SHIFT_UNSIGNED(UW, td_uw);
  default:
    err_puts("tag_shift: invalid left operand type");
    return NULL;
  }
#undef TAG_SHIFT_SIGNED
#undef TAG_SHIFT_UNSIGNED
  mask = width == 64 ? U64_MAX : ((u64) 1 << width) - 1;
  bits &= mask;
  if (count >= width)
    bits = right && negative ? mask : 0;
  else if (right) {
    bits >>= count;
    if (negative)
      bits |= mask ^ (mask >> count);
  }
  else
    bits = (bits << count) & mask;
  switch (a->type) {
  case TAG_BOOL: tmp.data.td_bool_ = (bool) bits; break;
  case TAG_CHARACTER: tmp.data.td_character = (character) bits; break;
  case TAG_S8: tmp.data.td_s8 = (s8) tag_shift_signed(bits, mask); break;
  case TAG_S16: tmp.data.td_s16 = (s16) tag_shift_signed(bits, mask); break;
  case TAG_S32: tmp.data.td_s32 = (s32) tag_shift_signed(bits, mask); break;
  case TAG_S64: tmp.data.td_s64 = tag_shift_signed(bits, mask); break;
  case TAG_SW: tmp.data.td_sw = (sw) tag_shift_signed(bits, mask); break;
  case TAG_U8: tmp.data.td_u8 = (u8) bits; break;
  case TAG_U16: tmp.data.td_u16 = (u16) bits; break;
  case TAG_U32: tmp.data.td_u32 = (u32) bits; break;
  case TAG_U64: tmp.data.td_u64 = bits; break;
  case TAG_UW: tmp.data.td_uw = (uw) bits; break;
  default: return NULL;
  }
  dest->type = a->type;
  dest->data = tmp.data;
  return dest;
}

static s_tag * tag_shift_apply (const s_tag *a, const s_tag *b, bool right,
                                s_tag *dest)
{
  u64 count;
  sw integer_count;
  s_integer tmp;
  assert(a);
  assert(b);
  assert(dest);
  if (! tag_shift_count(b, &count)) {
    err_puts("tag_shift: invalid or negative shift count");
    return NULL;
  }
  if (a->type != TAG_INTEGER)
    return tag_shift_fixed(a, count, right, dest);
  /* SW_MAX already exceeds every supported nonzero Integer left shift.
     Saturation also lets zero left shifts and large right shifts finish
     without allocating storage proportional to the count. */
  integer_count = count > (u64) SW_MAX ? SW_MAX : (sw) count;
  if (right) {
    if (! integer_rshift(&a->data.td_integer, integer_count, &tmp))
      return NULL;
  }
  else if (! integer_lshift(&a->data.td_integer, integer_count, &tmp))
    return NULL;
  dest->type = TAG_INTEGER;
  dest->data.td_integer = tmp;
  return dest;
}

#endif /* LIBKC3_TAG_SHIFT_H */
