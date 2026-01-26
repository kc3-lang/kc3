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

s_tag * tag_bxor (s_tag *a, s_tag *b, s_tag *result)
{
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  assert(a);
  assert(b);
  assert(result);
  switch (a->type) {
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_INTEGER:
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &b->data.integer,
                   &result->data.integer);
      return result;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_sw(result, a->data.sw ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, a->data.sw ^ b->data.s64);
    case TAG_S32:
      return tag_init_sw(result, a->data.sw ^ b->data.s32);
    case TAG_S16:
      return tag_init_sw(result, a->data.sw ^ b->data.s16);
    case TAG_S8:
      return tag_init_sw(result, a->data.sw ^ b->data.s8);
    case TAG_U8:
      return tag_init_sw(result, a->data.sw ^ b->data.u8);
    case TAG_U16:
      return tag_init_sw(result, a->data.sw ^ b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, a->data.sw ^ b->data.u32);
    case TAG_U64:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_u64(&tmp2, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_UW:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_uw(&tmp2, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_s64(result, a->data.s64 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, a->data.s64 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s64(result, a->data.s64 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s64(result, a->data.s64 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s64(result, a->data.s64 ^ b->data.s8);
    case TAG_U8:
      return tag_init_s64(result, a->data.s64 ^ b->data.u8);
    case TAG_U16:
      return tag_init_s64(result, a->data.s64 ^ b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, a->data.s64 ^ b->data.u32);
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_UW:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_uw(&tmp2, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_sw(result, (sw) a->data.s32 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, (s64) a->data.s32 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, a->data.s32 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s32(result, a->data.s32 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s32(result, a->data.s32 ^ b->data.s8);
    case TAG_U8:
      return tag_init_s32(result, a->data.s32 ^ b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, a->data.s32 ^ b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, (s64) a->data.s32 ^ b->data.u32);
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_UW:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_uw(&tmp2, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_sw(result, (sw) a->data.s16 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, (s64) a->data.s16 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, (s32) a->data.s16 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, a->data.s16 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, a->data.s16 ^ b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, a->data.s16 ^ b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, (s32) a->data.s16 ^ b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, (s64) a->data.s16 ^ b->data.u32);
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_UW:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_uw(&tmp2, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_sw(result, (sw) a->data.s8 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, (s64) a->data.s8 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, (s32) a->data.s8 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, (s16) a->data.s8 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s8(result, a->data.s8 ^ b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, (s16) a->data.s8 ^ b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, (s32) a->data.s8 ^ b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, (s64) a->data.s8 ^ b->data.u32);
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_UW:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_uw(&tmp2, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 ^
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u8 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_sw(result, (sw) a->data.u8 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, (s64) a->data.u8 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, (s32) a->data.u8 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, (s16) a->data.u8 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, (s16) a->data.u8 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u8(result, a->data.u8 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, (u16) a->data.u8 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, (u32) a->data.u8 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, (u64) a->data.u8 ^ b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, (uw) a->data.u8 ^ b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 ^
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u16 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_sw(result, (sw) a->data.u16 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, (s64) a->data.u16 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, (s32) a->data.u16 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s32(result, (s32) a->data.u16 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s32(result, (s32) a->data.u16 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u16(result, a->data.u16 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, (u16) a->data.u16 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, (u32) a->data.u16 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, (u64) a->data.u16 ^ b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, (uw) a->data.u16 ^ b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 ^
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u32 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      return tag_init_s64(result, (s64) a->data.u32 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, (s64) a->data.u32 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s64(result, (s64) a->data.u32 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s64(result, (s64) a->data.u32 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s64(result, (s64) a->data.u32 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u32(result, a->data.u32 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u32(result, (u32) a->data.u32 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, (u32) a->data.u32 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, (u64) a->data.u32 ^ b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, (uw) a->data.u32 ^ b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 ^
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u64(result, a->data.u64 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &b->data.integer, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_SW:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_sw(&tmp2, b->data.sw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_U8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u8(&tmp2, b->data.u8);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_U16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u16(&tmp2, b->data.u16);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_U32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u32(&tmp2, b->data.u32);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_U64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u64(&tmp2, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_UW:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_uw(&tmp2, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bxor(&tmp, &tmp2, &result->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw ^
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_uw(result, a->data.uw ^ b->data.character);
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_uw(result, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_uw(result, a->data.uw ^ b->data.sw);
    case TAG_S64:
      return tag_init_uw(result, a->data.uw ^ b->data.s64);
    case TAG_S32:
      return tag_init_uw(result, a->data.uw ^ b->data.s32);
    case TAG_S16:
      return tag_init_uw(result, a->data.uw ^ b->data.s16);
    case TAG_S8:
      return tag_init_uw(result, a->data.uw ^ b->data.s8);
    case TAG_U8:
      return tag_init_uw(result, a->data.uw ^ b->data.u8);
    case TAG_U16:
      return tag_init_uw(result, a->data.uw ^ b->data.u16);
    case TAG_U32:
      return tag_init_uw(result, a->data.uw ^ b->data.u32);
    case TAG_U64:
      return tag_init_uw(result, a->data.uw ^ b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, a->data.uw ^ b->data.uw);
    default:
      goto error;
    }
    goto error;
  default:
    goto error;
  }
 error:
  err_write_1("tag_bxor: invalid tag type: ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" ^ ");
  err_puts(tag_type_to_string(b->type));
  assert(! "tag_bxor: invalid tag type");
  return NULL;
}
