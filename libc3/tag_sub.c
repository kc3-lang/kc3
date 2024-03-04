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
#include "assert.h"
#include <math.h>
#include "integer.h"
#include "ratio.h"
#include "tag.h"

s_tag * tag_sub (const s_tag *a, const s_tag *b, s_tag *dest)
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
      return tag_init_f32(dest, a->data.f32 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f32 - b->data.f64);
    case TAG_INTEGER:
      return tag_init_f32(dest, a->data.f32 -
                          integer_to_f32(&b->data.integer));
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 - b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 - b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 - b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 - b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 - b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 - b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 - b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 - b->data.f64);
    case TAG_INTEGER:
      return tag_init_f64(dest, a->data.f64 -
                          integer_to_f64(&b->data.integer));
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 - b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 - b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 - b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 - b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 - b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 - b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 - b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, integer_to_f64(&a->data.integer) -
                          b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, integer_to_f64(&a->data.integer) -
                          b->data.f64);
    case TAG_INTEGER:
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &b->data.integer,
                  &dest->data.integer);
      return dest;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    default:
      goto ko;
    }
  case TAG_RATIO:
    switch (b->type) {
    case TAG_RATIO:
      if (! ratio_sub(&a->data.ratio, &b->data.ratio,
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
      return tag_init_f32(dest, a->data.s8 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s8 - b->data.f64);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, (s16) a->data.s8 - (s16) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.s8 - (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.s8 - (s64) b->data.s32);
    case TAG_S64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s16(dest, (s16) a->data.s8 - (s16) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, (s32) a->data.s8 - (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.s8 - (s64) b->data.u32);
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s16 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s16 - b->data.f64);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, a->data.s16 - b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s16 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s16 - b->data.s32);
    case TAG_S64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s32(dest, (s32) a->data.s16 - (s32) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, (s32) a->data.s16 - (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.s16 - (s64) b->data.u32);
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s32 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s32 - b->data.f64);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s64(dest, (s64) a->data.s32 - (s64) b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, (s64) a->data.s32 - (s64) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.s32 - (s64) b->data.s32);
    case TAG_S64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s64(dest, (s64) a->data.s32 - (s64) b->data.u8);
    case TAG_U16:
      return tag_init_s64(dest, (s64) a->data.s32 - (s64) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.s32 - (s64) b->data.u32);
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s64 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s64 - b->data.f64);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u8(&tmp2, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U16:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u16(&tmp2, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U32:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u32(&tmp2, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u8 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u8 - b->data.f64);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, (s16) a->data.u8 - (s16) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.u8 - (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.u8 - (s64) b->data.s32);
    case TAG_S64:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s16(dest, (s16) a->data.u8 - (s16) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, (s32) a->data.u8 - (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.u8 - (s64) b->data.u32);
    case TAG_U64:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u16 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u16 - b->data.f64);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s32(dest, (s32) a->data.u16 - (s32) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.u16 - (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.u16 - (s64) b->data.s32);
    case TAG_S64:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s32(dest, (s32) a->data.u16 - (s32) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, (s32) a->data.u16 - (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.u16 - (s64) b->data.u32);
    case TAG_U64:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u32 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u32 - b->data.f64);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s64(dest, (s64) a->data.u32 - (s64) b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, (s64) a->data.u32 - (s64) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.u32 - (s64) b->data.s32);
    case TAG_S64:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s64(dest, (s64) a->data.u32 - (s64) b->data.u8);
    case TAG_U16:
      return tag_init_s64(dest, (s64) a->data.u32 - (s64) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.u32 - (s64) b->data.u32);
    case TAG_U64:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u64 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u64 - b->data.f64);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u8(&tmp2, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u16(&tmp2, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u32(&tmp2, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_sub(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  err_write_1("tag_sub: cannot subtract ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" by ");
  err_puts(tag_type_to_string(b->type));
  return NULL;
}
