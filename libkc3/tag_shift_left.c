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

// FIXME
s_tag * tag_shift_left (s_tag *a, s_tag *b, s_tag *result)
{
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  s_tag tmp_a;
  switch (a->type) {
  case TAG_BOOL:
    tmp_a.data.bool_ = a->data.bool_ ? 1 : 0;
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_bool(result, tmp_a.data.bool_ <<
                           (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_bool(result,
                           tmp_a.data.bool_ << b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, tmp_a.data.bool_);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_bool(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.sw);
    case TAG_S64:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.s64);
    case TAG_S32:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.s32);
    case TAG_S16:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.s16);
    case TAG_S8:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.s8);
    case TAG_U8:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.u8);
    case TAG_U16:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.u16);
    case TAG_U32:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.u32);
    case TAG_U64:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.u64);
    case TAG_UW:
      return tag_init_bool(result, tmp_a.data.bool_ << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_CHARACTER:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_character(result, a->data.character <<
                                (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_character(result,
                                a->data.character << b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.character);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_character(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_character(result,
                                a->data.character << b->data.sw);
    case TAG_S64:
      return tag_init_character(result,
                                a->data.character << b->data.s64);
    case TAG_S32:
      return tag_init_character(result,
                                a->data.character << b->data.s32);
    case TAG_S16:
      return tag_init_character(result,
                                a->data.character << b->data.s16);
    case TAG_S8:
      return tag_init_character(result,
                                a->data.character << b->data.s8);
    case TAG_U8:
      return tag_init_character(result,
                                a->data.character << b->data.u8);
    case TAG_U16:
      return tag_init_character(result,
                                a->data.character << b->data.u16);
    case TAG_U32:
      return tag_init_character(result,
                                a->data.character << b->data.u32);
    case TAG_U64:
      return tag_init_character(result,
                                a->data.character << b->data.u64);
    case TAG_UW:
      return tag_init_character(result,
                                a->data.character << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_BOOL:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.bool_ ? 1 : 0,
                     &result->data.integer);
      return result;
    case TAG_CHARACTER:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.character,
                     &result->data.integer);
      return result;
    case TAG_INTEGER:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, integer_to_sw(&b->data.integer),
                     &result->data.integer);
      return result;
    case TAG_SW:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.sw,
                     &result->data.integer);
      return result;
    case TAG_S64:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s64,
                     &result->data.integer);
      return result;
    case TAG_S32:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s32,
                     &result->data.integer);
      return result;
    case TAG_S16:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s16,
                     &result->data.integer);
      return result;
    case TAG_S8:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s8,
                     &result->data.integer);
      return result;
    case TAG_U8:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u8,
                     &result->data.integer);
      return result;
    case TAG_U16:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u16,
                     &result->data.integer);
      return result;
    case TAG_U32:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u32,
                     &result->data.integer);
      return result;
    case TAG_U64:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u64,
                     &result->data.integer);
      return result;
    case TAG_UW:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.uw,
                     &result->data.integer);
      return result;
    default:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_sw(result, a->data.sw <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_sw(result, a->data.sw << b->data.character);
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_sw(result, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_sw(result, a->data.sw << b->data.sw);
    case TAG_S64:
      return tag_init_sw(result, a->data.sw << b->data.s64);
    case TAG_S32:
      return tag_init_sw(result, a->data.sw << b->data.s32);
    case TAG_S16:
      return tag_init_sw(result, a->data.sw << b->data.s16);
    case TAG_S8:
      return tag_init_sw(result, a->data.sw << b->data.s8);
    case TAG_U8:
      return tag_init_sw(result, a->data.sw << b->data.u8);
    case TAG_U16:
      return tag_init_sw(result, a->data.sw << b->data.u16);
    case TAG_U32:
      return tag_init_sw(result, a->data.sw << b->data.u32);
    case TAG_U64:
      return tag_init_sw(result, a->data.sw << b->data.u64);
    case TAG_UW:
      return tag_init_sw(result, a->data.sw << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s64(result, a->data.s64 <<
                          (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s64(result, a->data.s64 << b->data.character);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s64(result, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s64(result, a->data.s64 << b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, a->data.s64 << b->data.s64);
    case TAG_S32:
      return tag_init_s64(result, a->data.s64 << b->data.s32);
    case TAG_S16:
      return tag_init_s64(result, a->data.s64 << b->data.s16);
    case TAG_S8:
      return tag_init_s64(result, a->data.s64 << b->data.s8);
    case TAG_U8:
      return tag_init_s64(result, a->data.s64 << b->data.u8);
    case TAG_U16:
      return tag_init_s64(result, a->data.s64 << b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, a->data.s64 << b->data.u32);
    case TAG_U64:
      return tag_init_s64(result, a->data.s64 << b->data.u64);
    case TAG_UW:
      return tag_init_s64(result, a->data.s64 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s32(result, a->data.s32 <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s32(result, a->data.s32 << b->data.character);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s32(result, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s32(result, a->data.s32 << b->data.sw);
    case TAG_S64:
      return tag_init_s32(result, a->data.s32 << b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, a->data.s32 << b->data.s32);
    case TAG_S16:
      return tag_init_s32(result, a->data.s32 << b->data.s16);
    case TAG_S8:
      return tag_init_s32(result, a->data.s32 << b->data.s8);
    case TAG_U8:
      return tag_init_s32(result, a->data.s32 << b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, a->data.s32 << b->data.u16);
    case TAG_U32:
      return tag_init_s32(result, a->data.s32 << b->data.u32);
    case TAG_U64:
      return tag_init_s32(result, a->data.s32 << b->data.u64);
    case TAG_UW:
      return tag_init_s32(result, a->data.s32 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s16(result, a->data.s16 <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s16(result, a->data.s16 << b->data.character);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s16(result, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s16(result, a->data.s16 << b->data.sw);
    case TAG_S64:
      return tag_init_s16(result, a->data.s16 << b->data.s64);
    case TAG_S32:
      return tag_init_s16(result, a->data.s16 << b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, a->data.s16 << b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, a->data.s16 << b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, a->data.s16 << b->data.u8);
    case TAG_U16:
      return tag_init_s16(result, a->data.s16 << b->data.u16);
    case TAG_U32:
      return tag_init_s16(result, a->data.s16 << b->data.u32);
    case TAG_U64:
      return tag_init_s16(result, a->data.s16 << b->data.u64);
    case TAG_UW:
      return tag_init_s16(result, a->data.s16 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s8(result, a->data.s8 <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s8(result, a->data.s8 << b->data.character);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s8(result, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s8(result, a->data.s8 << b->data.sw);
    case TAG_S64:
      return tag_init_s8(result, a->data.s8 << b->data.s64);
    case TAG_S32:
      return tag_init_s8(result, a->data.s8 << b->data.s32);
    case TAG_S16:
      return tag_init_s8(result, a->data.s8 << b->data.s16);
    case TAG_S8:
      return tag_init_s8(result, a->data.s8 << b->data.s8);
    case TAG_U8:
      return tag_init_s8(result, a->data.s8 << b->data.u8);
    case TAG_U16:
      return tag_init_s8(result, a->data.s8 << b->data.u16);
    case TAG_U32:
      return tag_init_s8(result, a->data.s8 << b->data.u32);
    case TAG_U64:
      return tag_init_s8(result, a->data.s8 << b->data.u64);
    case TAG_UW:
      return tag_init_s8(result, a->data.s8 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u8(result, a->data.u8 << b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u8(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u8(result, a->data.u8 << b->data.sw);
    case TAG_S64:
      return tag_init_u8(result, a->data.u8 << b->data.s64);
    case TAG_S32:
      return tag_init_u8(result, a->data.u8 << b->data.s32);
    case TAG_S16:
      return tag_init_u8(result, a->data.u8 << b->data.s16);
    case TAG_S8:
      return tag_init_u8(result, a->data.u8 << b->data.s8);
    case TAG_U8:
      return tag_init_u8(result, a->data.u8 << b->data.u8);
    case TAG_U16:
      return tag_init_u8(result, a->data.u8 << b->data.u16);
    case TAG_U32:
      return tag_init_u8(result, a->data.u8 << b->data.u32);
    case TAG_U64:
      return tag_init_u8(result, a->data.u8 << b->data.u64);
    case TAG_UW:
      return tag_init_u8(result, a->data.u8 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u16(result, a->data.u16 << b->data.character);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u16(result, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u16(result, a->data.u16 << b->data.sw);
    case TAG_S64:
      return tag_init_u16(result, a->data.u16 << b->data.s64);
    case TAG_S32:
      return tag_init_u16(result, a->data.u16 << b->data.s32);
    case TAG_S16:
      return tag_init_u16(result, a->data.u16 << b->data.s16);
    case TAG_S8:
      return tag_init_u16(result, a->data.u16 << b->data.s8);
    case TAG_U8:
      return tag_init_u16(result, a->data.u16 << b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, a->data.u16 << b->data.u16);
    case TAG_U32:
      return tag_init_u16(result, a->data.u16 << b->data.u32);
    case TAG_U64:
      return tag_init_u16(result, a->data.u16 << b->data.u64);
    case TAG_UW:
      return tag_init_u16(result, a->data.u16 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 <<
                          (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u32 << b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u32(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u32(result, a->data.u32 << b->data.sw);
    case TAG_S64:
      return tag_init_u32(result, a->data.u32 << b->data.s64);
    case TAG_S32:
      return tag_init_u32(result, a->data.u32 << b->data.s32);
    case TAG_S16:
      return tag_init_u32(result, a->data.u32 << b->data.s16);
    case TAG_S8:
      return tag_init_u32(result, a->data.u32 << b->data.s8);
    case TAG_U8:
      return tag_init_u32(result, a->data.u32 << b->data.u8);
    case TAG_U16:
      return tag_init_u32(result, a->data.u32 << b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, a->data.u32 << b->data.u32);
    case TAG_U64:
      return tag_init_u32(result, a->data.u32 << b->data.u64);
    case TAG_UW:
      return tag_init_u32(result, a->data.u32 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u64(result, a->data.u64 << b->data.character);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u64(result, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u64(result, a->data.u64 << b->data.sw);
    case TAG_S64:
      return tag_init_u64(result, a->data.u64 << b->data.s64);
    case TAG_S32:
      return tag_init_u64(result, a->data.u64 << b->data.s32);
    case TAG_S16:
      return tag_init_u64(result, a->data.u64 << b->data.s16);
    case TAG_S8:
      return tag_init_u64(result, a->data.u64 << b->data.s8);
    case TAG_U8:
      return tag_init_u64(result, a->data.u64 << b->data.u8);
    case TAG_U16:
      return tag_init_u64(result, a->data.u64 << b->data.u16);
    case TAG_U32:
      return tag_init_u64(result, a->data.u64 << b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, a->data.u64 << b->data.u64);
    case TAG_UW:
      return tag_init_u64(result, a->data.u64 << b->data.uw);
    default:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw <<
                         (b->data.bool_ ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_uw(result, a->data.uw << b->data.character);
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_uw(result, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_uw(result, a->data.uw << b->data.sw);
    case TAG_S64:
      return tag_init_uw(result, a->data.uw << b->data.s64);
    case TAG_S32:
      return tag_init_uw(result, a->data.uw << b->data.s32);
    case TAG_S16:
      return tag_init_uw(result, a->data.uw << b->data.s16);
    case TAG_S8:
      return tag_init_uw(result, a->data.uw << b->data.s8);
    case TAG_U8:
      return tag_init_uw(result, a->data.uw << b->data.u8);
    case TAG_U16:
      return tag_init_uw(result, a->data.uw << b->data.u16);
    case TAG_U32:
      return tag_init_uw(result, a->data.uw << b->data.u32);
    case TAG_U64:
      return tag_init_uw(result, a->data.uw << b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, a->data.uw << b->data.uw);
    default:
      goto error;
    }
    goto error;
  default:
    goto error;
  }
 error:
  err_write_1("tag_shift_left: invalid tag type: ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" << ");
  err_puts(tag_type_to_string(b->type));
  assert(! "tag_shift_left: invalid tag type");
  return NULL;
}
