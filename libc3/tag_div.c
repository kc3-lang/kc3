/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include <math.h>
#include "integer.h"
#include "ratio.h"
#include "tag.h"

s_tag * tag_div (const s_tag *a, const s_tag *b, s_tag *dest)
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
      return tag_init_f32(dest, a->data.f32 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.f32 / b->data.f64);
    case TAG_INTEGER:
      return tag_init_f32(dest, a->data.f32 /
                          integer_to_f32(&b->data.integer));
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 / (f32) b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 / b->data.f64);
    case TAG_INTEGER:
      return tag_init_f64(dest, a->data.f64 /
                          integer_to_f64(&b->data.integer));
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 / (f64) b->data.u64);
    default:
      goto ko;
  }
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) integer_to_f64(&a->data.integer) /
                          b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, integer_to_f64(&a->data.integer) /
                          b->data.f64);
    case TAG_INTEGER:
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &b->data.integer,
                  &dest->data.integer);
      return dest;
    case TAG_S8:
      integer_init_s32(&tmp, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S16:
      integer_init_s32(&tmp, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U8:
      integer_init_u32(&tmp, (u32) b->data.u8);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U16:
      integer_init_u32(&tmp, (u32) b->data.u16);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_div(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    default:
      goto ko;
    }
  case TAG_RATIO:
    switch (b->type) {
    case TAG_RATIO:
      if (! ratio_div(&a->data.ratio, &b->data.ratio,
                      &dest->data.ratio))
        return NULL;
      dest->type = TAG_RATIO;
      return dest;
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s8 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s8 / b->data.f64);
    case TAG_INTEGER:
      integer_init_s32(&tmp, (s32) a->data.s8);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s8(dest, a->data.s8 / b->data.s8);
    case TAG_S16:
      return tag_init_s8(dest, (s8) ((s16) a->data.s8 / b->data.s16));
    case TAG_S32:
      return tag_init_s8(dest, (s8) ((s32) a->data.s8 / b->data.s32));
    case TAG_S64:
      return tag_init_s8(dest, (s8) ((s64) a->data.s8 / b->data.s64));
    case TAG_U8:
      return tag_init_s8(dest, (s8) ((s16) a->data.s8 / (s16) b->data.u8));
    case TAG_U16:
      return tag_init_s8(dest, (s8) ((s32) a->data.s8 / (s32) b->data.u16));
    case TAG_U32:
      return tag_init_s8(dest, (s8) ((s64) a->data.s8 / (s64) b->data.u32));
    case TAG_U64:
      if (b->data.u64 > S8_MAX)
        return tag_init_s8(dest, 0);
      else
        return tag_init_s8(dest, a->data.s8 / (s8) b->data.u64);
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s16 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s16 / b->data.f64);
    case TAG_INTEGER:
      integer_init_s32(&tmp, (s32) a->data.s16);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, a->data.s16 / (s16) b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s16 / b->data.s16);
    case TAG_S32:
      return tag_init_s16(dest, ((s16) ((s32) a->data.s16 / b->data.s32)));
    case TAG_S64:
      return tag_init_s16(dest, ((s16) ((s64) a->data.s16 / b->data.s64)));
    case TAG_U8:
      return tag_init_s16(dest, a->data.s16 / (s16) b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, (s16) ((s32) a->data.s16 / (s32) b->data.u16));
    case TAG_U32:
      return tag_init_s16(dest, (s16) ((s64) a->data.s16 / (s64) b->data.u32));
    case TAG_U64:
      if (b->data.u64 > S16_MAX)
        return tag_init_s16(dest, 0);
      else
        return tag_init_s16(dest, (s16) a->data.s16 / (s16) b->data.u64);
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s32 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s32 / b->data.f64);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s32(dest, a->data.s32 / (s32) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s32 / (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s32 / b->data.s32);
    case TAG_S64:
      return tag_init_s32(dest, (s32) ((s64) a->data.s32 / b->data.s64));
    case TAG_U8:
      return tag_init_s32(dest, a->data.s32 / (s32) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s32 / (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s32 / (s64) b->data.u32);
    case TAG_U64:
      if (b->data.u64 > S32_MAX)
        return tag_init_s32(dest, 0);
      else
        return tag_init_s32(dest, a->data.s32 / (s32) b->data.u64);
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s64 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s64 / b->data.f64);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s64(dest, a->data.s64 / (s64) b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, a->data.s64 / (s64) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, a->data.s64 / (s64) b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s64 / b->data.s64);
    case TAG_U8:
      return tag_init_s64(dest, a->data.s64 / (s64) b->data.u8);
    case TAG_U16:
      return tag_init_s64(dest, a->data.s64 / (s64) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, a->data.s64 / (s64) b->data.u32);
    case TAG_U64:
      if (b->data.u64 > S64_MAX)
        return tag_init_s64(dest, 0);
      else
        return tag_init_s64(dest, a->data.s64 / (s64) b->data.u64);
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u8 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u8 / b->data.f64);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u8);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, (s16) a->data.u8 / (s16) b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, (s16) a->data.u8 / b->data.s16);
    case TAG_S32:
      return tag_init_s16(dest, (s16) ((s32) a->data.u8 / b->data.s32));
    case TAG_S64:
      return tag_init_s16(dest, (s16) ((s64) a->data.u8 / b->data.s64));
    case TAG_U8:
      return tag_init_u8(dest, a->data.u8 / b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, (u8) ((u16) a->data.u8 / b->data.u16));
    case TAG_U32:
      return tag_init_u32(dest, (u8) ((u32) a->data.u8 / b->data.u32));
    case TAG_U64:
      return tag_init_u64(dest, (u8) ((u64) a->data.u8 / b->data.u64));
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u16 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u16 / b->data.f64);
    case TAG_INTEGER:
      integer_init_u32(&tmp, (u32) a->data.u16);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s32(dest, (s32) a->data.u16 / (s32) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.u16 / (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, (s32) a->data.u16 / b->data.s32);
    case TAG_S64:
      return tag_init_s32(dest, (s32) ((s64) a->data.u16 /
                                       b->data.s64));
    case TAG_U8:
      return tag_init_u16(dest, a->data.u16 / (u16) b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u16 / b->data.u16);
    case TAG_U32:
      return tag_init_u16(dest, (u16) ((u32) a->data.u16 / b->data.u32));
    case TAG_U64:
      return tag_init_u16(dest, (u16) ((u64) a->data.u16 / b->data.u64));
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u32 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u32 / b->data.f64);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s64(dest, (s64) a->data.u32 / (s64) b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, (s64) a->data.u32 / (s64) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.u32 / (s64) b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, (s64) a->data.u32 / b->data.s64);
    case TAG_U8:
      return tag_init_u32(dest, a->data.u32 / b->data.u8);
    case TAG_U16:
      return tag_init_u32(dest, a->data.u32 / b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u32 / b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u32 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u64 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u64 / b->data.f64);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      dest->type = TAG_INTEGER;
      integer_div(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      if (a->data.u64 > S64_MAX) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_s32(&tmp2, (s32) b->data.s8);
        dest->type = TAG_INTEGER;
        integer_div(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u64 / (s64) b->data.s8);
    case TAG_S16:
      if (a->data.u64 > S64_MAX) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_s32(&tmp2, (s32) b->data.s16);
        dest->type = TAG_INTEGER;
        integer_div(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u64 /
                            (s64) b->data.s16);
    case TAG_S32:
      if (a->data.u64 > S64_MAX) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_s32(&tmp2, b->data.s32);
        dest->type = TAG_INTEGER;
        integer_div(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u64 /
                            (s64) b->data.s32);
    case TAG_S64:
      if (a->data.u64 > S64_MAX) {
        integer_init_u64(&tmp, a->data.u64);
        integer_init_s64(&tmp2, b->data.s64);
        dest->type = TAG_INTEGER;
        integer_div(&tmp, &tmp2, &dest->data.integer);
        integer_clean(&tmp);
        integer_clean(&tmp2);
        return dest;
      }
      else
        return tag_init_s64(dest, (s64) a->data.u64 / b->data.s64);
    case TAG_U8:
      return tag_init_u64(dest, a->data.u64 / (u64) b->data.u8);
    case TAG_U16:
      return tag_init_u64(dest, a->data.u64 / (u64) b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, a->data.u64 / (u64) b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u64 / b->data.u64);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  err_write_1("tag_div: invalid tag type: ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" / ");
  err_puts(tag_type_to_string(b->type));
  assert(! "tag_div: invalid tag type");
  return NULL;
}
