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
#include <assert.h>
#include <err.h>
#include <math.h>
#include "integer.h"
#include "tag.h"

s_tag * tag_add (const s_tag *a, const s_tag *b, s_tag *dest)
{
  s_integer tmp;
  s_integer tmp2;
  assert(a);
  assert(b);
  assert(dest);
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.f32 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.f32 + b->data.f64);
    case TAG_INTEGER:
      return tag_init_f32(dest, a->data.f32 +
                          integer_to_f32(&a->data.integer));
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.s64);
    case TAG_SW:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.sw);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.u64);
    case TAG_UW:
      return tag_init_f32(dest, a->data.f32 + (f32) b->data.uw);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 + b->data.f64);
    case TAG_INTEGER:
      return tag_init_f64(dest, a->data.f64 +
                          integer_to_f64(&a->data.integer));
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.s64);
    case TAG_SW:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.sw);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.u64);
    case TAG_UW:
      return tag_init_f64(dest, a->data.f64 + (f64) b->data.uw);
    default:
      goto ko;
  }
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) integer_to_f64(&a->data.integer) +
                          b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, integer_to_f64(&a->data.integer) +
                          b->data.f64);
    case TAG_INTEGER:
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &b->data.integer,
                  &dest->data.integer);
      return dest;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s8 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s8 + b->data.f64);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.s8 < S8_MIN - b->data.s8 ||
          a->data.s8 > S8_MAX - b->data.s8)
        return tag_init_s16(dest, (s16) a->data.s8 + (s16) b->data.s8);
      else
        return tag_init_s8(dest, a->data.s8 + b->data.s8);
    case TAG_S16:
      if (a->data.s8 < S16_MIN - b->data.s16 ||
          a->data.s8 > S16_MAX - b->data.s16)
        return tag_init_s32(dest, (s32) a->data.s8 + (s32) b->data.s16);
      else
        return tag_init_s16(dest, (s16) a->data.s8 + b->data.s16);
    case TAG_S32:
      if (a->data.s8 < S32_MIN - b->data.s32 ||
          a->data.s8 > S32_MAX - b->data.s32)
        return tag_init_s64(dest, (s64) a->data.s8 + (s64) b->data.s32);
      else
        return tag_init_s32(dest, (s32) a->data.s8 + b->data.s32);
    case TAG_S64:
      if (a->data.s8 < S64_MIN - b->data.s64 ||
          a->data.s8 > S64_MAX - b->data.s64) {
        integer_init_s8(&tmp, a->data.s8);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.s8 + b->data.s64);
    case TAG_SW:
      if (a->data.s8 < SW_MIN - b->data.sw ||
          a->data.s8 > SW_MAX - b->data.sw) {
        integer_init_s8(&tmp, a->data.s8);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, (sw) a->data.s8 + b->data.sw);
    case TAG_U8:
      if (a->data.s8 < S8_MIN - b->data.u8 ||
          a->data.s8 > S8_MAX - b->data.u8)
        return tag_init_s16(dest, (s16) a->data.s8 + (s16) b->data.u8);
      else
        return tag_init_s8(dest, a->data.s8 + (s8) b->data.u8);
    case TAG_U16:
      if (a->data.s8 < S16_MIN - b->data.u16 ||
          a->data.s8 > S16_MAX - b->data.u16)
        return tag_init_s32(dest, (s32) a->data.s8 + (s32) b->data.u16);
      else
        return tag_init_s16(dest, (s16) a->data.s8 + (s16) b->data.u16);
    case TAG_U32:
      if (a->data.s8 < (s64) S32_MIN - (s64) b->data.u32 ||
          a->data.s8 > (s64) S32_MAX - (s64) b->data.u32)
        return tag_init_s64(dest, (s64) a->data.s8 + (s64) b->data.u32);
      else
        return tag_init_s32(dest, (s32) a->data.s8 + (s32) b->data.u32);
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_UW:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s16 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s16 + b->data.f64);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.s16 < S16_MIN - b->data.s8 ||
          a->data.s16 > S16_MAX - b->data.s8)
        return tag_init_s32(dest, (s32) a->data.s16 + (s32) b->data.s8);
      else
        return tag_init_s16(dest, a->data.s16 + (s16) b->data.s8);
    case TAG_S16:
      if (a->data.s16 < S16_MIN - b->data.s16 ||
          a->data.s16 > S16_MAX - b->data.s16)
        return tag_init_s32(dest, (s32) a->data.s16 + (s32) b->data.s16);
      else
        return tag_init_s16(dest, a->data.s16 + b->data.s16);
    case TAG_S32:
      if (a->data.s16 < S32_MIN - b->data.s32 ||
          a->data.s16 > S32_MAX - b->data.s32)
        return tag_init_s64(dest, (s64) a->data.s16 + (s64) b->data.s32);
      else
        return tag_init_s32(dest, (s32) a->data.s16 + b->data.s32);
    case TAG_S64:
      if (a->data.s16 < S64_MIN - b->data.s64 ||
          a->data.s16 > S64_MAX - b->data.s64) {
        integer_init_s16(&tmp, a->data.s16);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.s16 + b->data.s64);
    case TAG_SW:
      if (a->data.s16 < SW_MIN - b->data.sw ||
          a->data.s16 > SW_MAX - b->data.sw) {
        integer_init_s16(&tmp, a->data.s16);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.s16 + b->data.s64);
    case TAG_U8:
      if (a->data.s16 > S16_MAX - b->data.u8)
        return tag_init_s32(dest, (s32) a->data.s16 + (s32) b->data.u8);
      else
        return tag_init_s16(dest, a->data.s16 + (s16) b->data.u8);
    case TAG_U16:
      if (a->data.s16 > S16_MAX - b->data.u16)
        return tag_init_s32(dest, (s32) a->data.s16 + (s32) b->data.u16);
      else
        return tag_init_s16(dest, a->data.s16 + (s16) b->data.u16);
    case TAG_U32:
        return tag_init_s64(dest, (s64) a->data.s16 + (s64) b->data.u32);
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_UW:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
    }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s32 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s32 + b->data.f64);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.s32 < S32_MIN - b->data.s8 ||
          a->data.s32 > S32_MAX - b->data.s8)
        return tag_init_s64(dest, (s64) a->data.s32 + (s64) b->data.s8);
      else
        return tag_init_s32(dest, a->data.s32 + (s32) b->data.s8);
    case TAG_S16:
      if (a->data.s32 < S32_MIN - b->data.s16 ||
          a->data.s32 > S32_MAX - b->data.s16)
        return tag_init_s64(dest, (s64) a->data.s32 + (s64) b->data.s16);
      else
        return tag_init_s32(dest, a->data.s32 + (s32) b->data.s16);
    case TAG_S32:
      if (a->data.s32 < S32_MIN - b->data.s32 ||
          a->data.s32 > S32_MAX - b->data.s32)
        return tag_init_s64(dest, (s64) a->data.s32 + (s64) b->data.s32);
      else
        return tag_init_s32(dest, a->data.s32 + b->data.s32);
    case TAG_S64:
      if (a->data.s32 < S64_MIN - b->data.s64 ||
          a->data.s32 > S64_MAX - b->data.s64) {
        integer_init_s32(&tmp, a->data.s32);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.s32 + b->data.s64);
    case TAG_SW:
      if (a->data.s32 < SW_MIN - b->data.sw ||
          a->data.s32 > SW_MAX - b->data.sw) {
        integer_init_s32(&tmp, a->data.s32);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, (sw) a->data.s32 + b->data.sw);
    case TAG_U8:
      if (a->data.s32 > S32_MAX - b->data.u8)
        return tag_init_s64(dest, (s64) a->data.s32 + (s64) b->data.u8);
      else
        return tag_init_s32(dest, a->data.s32 + (s32) b->data.u8);
    case TAG_U16:
      if (a->data.s32 > S32_MAX - b->data.u16)
        return tag_init_s64(dest, (s64) a->data.s32 + (s64) b->data.u16);
      else
        return tag_init_s32(dest, a->data.s32 + (s32) b->data.u16);
    case TAG_U32:
      if (a->data.s32 > (s64) S32_MAX - (s64) b->data.u32)
        return tag_init_s64(dest, (s64) a->data.s32 + (s64) b->data.u32);
      else
        return tag_init_s32(dest, a->data.s32 + (s32) b->data.u32);
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_UW:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s64 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s64 + b->data.f64);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.s64 < S64_MIN - b->data.s8 ||
          a->data.s64 > S64_MAX - b->data.s8) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_s8(&tmp2, b->data.s8);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + (s64) b->data.s8);
    case TAG_S16:
      if (a->data.s64 < S64_MIN - b->data.s16 ||
          a->data.s64 > S64_MAX - b->data.s16) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_s16(&tmp2, b->data.s16);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + (s64) b->data.s16);
    case TAG_S32:
      if (a->data.s64 < S64_MIN - b->data.s32 ||
          a->data.s64 > S64_MAX - b->data.s32) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_s32(&tmp2, b->data.s32);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + (s64) b->data.s32);
    case TAG_S64:
      if (a->data.s64 < S64_MIN - b->data.s64 ||
          a->data.s64 > S64_MAX - b->data.s64) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + b->data.s64);
    case TAG_SW:
      if (a->data.s64 < S64_MIN - (s64) b->data.sw ||
          a->data.s64 > S64_MAX - (s64) b->data.sw) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + b->data.s64);
    case TAG_U8:
      if (a->data.s64 > S64_MAX - b->data.u8) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_u8(&tmp2, b->data.u8);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + (s64) b->data.u8);
    case TAG_U16:
      if (a->data.s64 > S64_MAX - b->data.u16) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_u16(&tmp2, b->data.u16);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + (s64) b->data.u16);
    case TAG_U32:
      if (a->data.s64 > S64_MAX - b->data.u32) {
        integer_init_s64(&tmp, a->data.s64);
        integer_init_u32(&tmp2, b->data.u32);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, a->data.s64 + (s64) b->data.u32);
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_UW:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_SW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.sw + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.sw + b->data.f64);
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.sw < SW_MIN - b->data.s8 ||
          a->data.sw > SW_MAX - b->data.s8) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_s8(&tmp2, b->data.s8);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + (sw) b->data.s8);
    case TAG_S16:
      if (a->data.sw < SW_MIN - b->data.s16 ||
          a->data.sw > SW_MAX - b->data.s16) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_s16(&tmp2, b->data.s16);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + (sw) b->data.s16);
    case TAG_S32:
      if (a->data.sw < SW_MIN - b->data.s32 ||
          a->data.sw > SW_MAX - b->data.s32) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_s32(&tmp2, b->data.s32);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + (sw) b->data.s32);
    case TAG_S64:
      if (a->data.sw < S64_MIN - b->data.s64 ||
          a->data.sw > S64_MAX - b->data.s64) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.sw + b->data.s64);
    case TAG_SW:
      if (a->data.sw < SW_MIN - b->data.sw ||
          a->data.sw > SW_MAX - b->data.sw) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + b->data.sw);
    case TAG_U8:
      if (a->data.sw > SW_MAX - b->data.u8) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_u8(&tmp2, b->data.u8);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + (sw) b->data.u8);
    case TAG_U16:
      if (a->data.sw > SW_MAX - b->data.u16) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_u16(&tmp2, b->data.u16);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + (sw) b->data.u16);
    case TAG_U32:
      if (a->data.sw > SW_MAX - b->data.u32) {
        integer_init_sw(&tmp, a->data.sw);
        integer_init_u32(&tmp2, b->data.u32);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, a->data.sw + (sw) b->data.u32);
    case TAG_U64:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_UW:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u8 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u8 + b->data.f64);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.u8 > S8_MAX - b->data.s8)
        return tag_init_s16(dest, (s16) a->data.u8 + (s16) b->data.s8);
      else
        return tag_init_s8(dest, (s8) a->data.u8 + b->data.s8);
    case TAG_S16:
      if (a->data.u8 > S16_MAX - b->data.s16)
        return tag_init_s32(dest, (s32) a->data.u8 + (s32) b->data.s16);
      else
        return tag_init_s16(dest, (s16) a->data.u8 + b->data.s16);
    case TAG_S32:
      if (a->data.u8 > S32_MAX - b->data.s32)
        return tag_init_s64(dest, (s64) a->data.u8 + (s64) b->data.s32);
      else
        return tag_init_s32(dest, (s32) a->data.u8 + b->data.s32);
    case TAG_S64:
      if (a->data.u8 > S64_MAX - b->data.s64) {
        integer_init_u8(&tmp, a->data.u8);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u8 + b->data.s64);
    case TAG_SW:
      if (a->data.u8 > SW_MAX - b->data.sw) {
        integer_init_u8(&tmp, a->data.u8);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u8 + b->data.s64);
    case TAG_U8:
      if (a->data.u8 > U8_MAX - b->data.u8)
        return tag_init_u16(dest, (u16) a->data.u8 + (u16) b->data.u8);
      else
        return tag_init_u8(dest, a->data.u8 + b->data.u8);
    case TAG_U16:
      if (a->data.u8 > U16_MAX - b->data.u16)
        return tag_init_u32(dest, (u32) a->data.u8 + (u32) b->data.u16);
      else
        return tag_init_u16(dest, (u16) a->data.u8 + b->data.u16);
    case TAG_U32:
      if (a->data.u8 > U32_MAX - b->data.u32)
        return tag_init_u64(dest, (u64) a->data.u8 + (u64) b->data.u32);
      else
        return tag_init_u32(dest, (u32) a->data.u8 + b->data.u32);
    case TAG_U64:
      if (a->data.u8 > U64_MAX - b->data.u64) {
        integer_init_u8(&tmp, a->data.u8);
        integer_init_u64(&tmp2, b->data.u64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, (u64) a->data.u8 + b->data.u64);
    case TAG_UW:
      if (a->data.u8 > UW_MAX - b->data.uw) {
        integer_init_u8(&tmp, a->data.u8);
        integer_init_uw(&tmp2, b->data.uw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, (uw) a->data.u8 + b->data.uw);
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u16 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u16 + b->data.f64);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.u16 > S16_MAX - b->data.s8)
        return tag_init_s32(dest, (s32) a->data.u16 + (s32) b->data.s8);
      else
        return tag_init_s16(dest, (s16) a->data.u16 + (s16) b->data.s8);
    case TAG_S16:
      if (a->data.u16 > S16_MAX - b->data.s16)
        return tag_init_s32(dest, (s32) a->data.u16 + (s32) b->data.s16);
      else
        return tag_init_s16(dest, (s16) a->data.u16 + b->data.s16);
    case TAG_S32:
      if (a->data.u16 > S32_MAX - b->data.s32)
        return tag_init_s64(dest, (s64) a->data.u16 + (s64) b->data.s32);
      else
        return tag_init_s32(dest, (s32) a->data.u16 + b->data.s32);
    case TAG_S64:
      if (a->data.u16 > S64_MAX - b->data.s64) {
        integer_init_u16(&tmp, a->data.u16);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u16 + b->data.s64);
    case TAG_SW:
      if (a->data.u16 > SW_MAX - b->data.sw) {
        integer_init_u16(&tmp, a->data.u16);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, (sw) a->data.u16 + b->data.sw);
    case TAG_U8:
      if (a->data.u16 > U16_MAX - b->data.u8)
        return tag_init_u32(dest, (u32) a->data.u16 + (u32) b->data.u8);
      else
        return tag_init_u16(dest, a->data.u16 + (u16) b->data.u8);
    case TAG_U16:
      if (a->data.u16 > U16_MAX - b->data.u16)
        return tag_init_u32(dest, (u32) a->data.u16 + (u32) b->data.u16);
      else
        return tag_init_u16(dest, a->data.u16 + b->data.u16);
    case TAG_U32:
      if (a->data.u16 > U32_MAX - b->data.u32)
        return tag_init_u64(dest, (u64) a->data.u16 + (u64) b->data.u32);
      else
        return tag_init_u32(dest, (u32) a->data.u16 + b->data.u32);
    case TAG_U64:
      if (a->data.u16 > U64_MAX - b->data.u64) {
        integer_init_u32(&tmp, (u32) a->data.u16);
        integer_init_u64(&tmp2, b->data.u64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, (u64) a->data.u16 + b->data.u64);
    case TAG_UW:
      if (a->data.u16 > UW_MAX - b->data.uw) {
        integer_init_u16(&tmp, a->data.u16);
        integer_init_uw(&tmp2, b->data.uw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, (uw) a->data.u16 + b->data.uw);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u32 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u32 + b->data.f64);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if ((s64) a->data.u32 > S32_MAX - b->data.s8)
        return tag_init_s64(dest, (s64) a->data.u32 + (s64) b->data.s8);
      else
        return tag_init_s32(dest, (s32) a->data.u32 + (s32) b->data.s8);
    case TAG_S16:
      if ((s64) a->data.u32 > S32_MAX - b->data.s16)
        return tag_init_s64(dest, (s64) a->data.u32 + (s64) b->data.s16);
      else
        return tag_init_s32(dest, (s32) a->data.u32 + (s32) b->data.s16);
    case TAG_S32:
      if ((s64) a->data.u32 > S32_MAX - b->data.s32)
        return tag_init_s64(dest, (s64) a->data.u32 + (s64) b->data.s32);
      else
        return tag_init_s32(dest, (s32) a->data.u32 + b->data.s32);
    case TAG_S64:
      if (a->data.u32 > S64_MAX - b->data.s64) {
        integer_init_u32(&tmp, a->data.u32);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u32 + b->data.s64);
    case TAG_SW:
      if (a->data.u32 > SW_MAX - b->data.sw) {
        integer_init_u32(&tmp, a->data.u32);
        integer_init_sw(&tmp2, b->data.sw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_sw(dest, (sw) a->data.u32 + b->data.sw);
    case TAG_U8:
      if (a->data.u32 > U32_MAX - b->data.u8)
        return tag_init_u64(dest, (u64) a->data.u32 + (u64) b->data.u8);
      else
        return tag_init_u32(dest, a->data.u32 + (u32) b->data.u8);
    case TAG_U16:
      if (a->data.u32 > U32_MAX - b->data.u16)
        return tag_init_u64(dest, (u64) a->data.u32 + (u64) b->data.u16);
      else
        return tag_init_u32(dest, a->data.u32 + (u32) b->data.u16);
    case TAG_U32:
      if (a->data.u32 > U32_MAX - b->data.u32)
        return tag_init_u64(dest, (u64) a->data.u32 + (u64) b->data.u32);
      else
        return tag_init_u32(dest, a->data.u32 + b->data.u32);
    case TAG_U64:
      if (a->data.u32 > U64_MAX - b->data.u64) {
        integer_init_u32(&tmp, a->data.u32);
        integer_init_u64(&tmp2, b->data.u64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, (u64) a->data.u32 + b->data.u64);
    case TAG_UW:
      if (a->data.u32 > UW_MAX - b->data.uw) {
        integer_init_u32(&tmp, a->data.u32);
        integer_init_uw(&tmp2, b->data.uw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, (uw) a->data.u32 + b->data.uw);
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u64 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u64 + b->data.f64);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_SW:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      if (a->data.u64 > U64_MAX - b->data.u8) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_u8(&tmp2, b->data.u8);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, a->data.u64 + (u64) b->data.u8);
    case TAG_U16:
      if (a->data.u64 > U64_MAX - b->data.u16) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_u16(&tmp2, b->data.u16);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, a->data.u64 + (u64) b->data.u16);
    case TAG_U32:
      if (a->data.u64 > U64_MAX - b->data.u32) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_u32(&tmp2, b->data.u32);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, a->data.u64 + (u64) b->data.u32);
    case TAG_U64:
      if (a->data.u64 > U64_MAX - b->data.u64) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_u64(&tmp2, b->data.u64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, a->data.u64 + b->data.u64);
    case TAG_UW:
      if (a->data.u64 > U64_MAX - b->data.uw) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_uw(&tmp2, b->data.uw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, a->data.u64 + b->data.uw);
    default:
      goto ko;
    }
  case TAG_UW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.uw + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.uw + b->data.f64);
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_SW:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      if (a->data.uw > UW_MAX - b->data.u8) {
        integer_init_uw(&tmp, a->data.uw);
        integer_init_u8(&tmp2, b->data.u8);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, a->data.uw + (uw) b->data.u8);
    case TAG_U16:
      if (a->data.uw > UW_MAX - b->data.u16) {
        integer_init_uw(&tmp, a->data.uw);
        integer_init_u16(&tmp2, b->data.u16);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, a->data.uw + (uw) b->data.u16);
    case TAG_U32:
      if (a->data.uw > UW_MAX - b->data.u32) {
        integer_init_uw(&tmp, a->data.uw);
        integer_init_u32(&tmp2, b->data.u32);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, a->data.uw + (uw) b->data.u32);
    case TAG_U64:
      if (a->data.uw > U64_MAX - b->data.u64) {
        integer_init_uw(&tmp, a->data.uw);
        integer_init_u64(&tmp2, b->data.u64);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_u64(dest, (u64) a->data.uw + b->data.u64);
    case TAG_UW:
      if (a->data.uw > UW_MAX - b->data.uw) {
        integer_init_uw(&tmp, a->data.uw);
        integer_init_uw(&tmp2, b->data.uw);
        dest->type = TAG_INTEGER;
        integer_add(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_uw(dest, a->data.uw + b->data.uw);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  errx(1, "cannot add %s to %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
}
