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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "compare.h"
#include "env.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "quote.h"
#include "str.h"
#include "tag.h"
#include "timespec.h"
#include "tuple.h"

s_tag g_tag_first;
s_tag g_tag_last;

s_tag * tag_1 (s_tag *tag, const s8 *p)
{
  tag_clean(tag);
  return tag_init_1(tag, p);
}

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

bool tag_and (const s_tag *a, const s_tag *b)
{
  s_tag f;
  tag_init_bool(&f, false);
  return compare_tag(a, &f) != 0 && compare_tag(b, &f) != 0;
}

s_tag * tag_array (s_tag *tag, const s_array *a)
{
  assert(tag);
  assert(a);
  tag_clean(tag);
  return tag_init_array(tag, a);
}

s_tag * tag_band (const s_tag *a, const s_tag *b, s_tag *result)
{
  s_integer tmp;
  s_integer tmp2;
  s_tag tmp_a;
  switch (a->type) {
  case TAG_BOOL:
    tmp_a.data.bool = a->data.bool ? 1 : 0;
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_bool(result, tmp_a.data.bool &
                           (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_bool(result, tmp_a.data.bool & b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, tmp_a.data.bool);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_bool(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_bool(result, tmp_a.data.bool & b->data.sw);
    case TAG_S64:
      return tag_init_bool(result, tmp_a.data.bool & b->data.s64);
    case TAG_S32:
      return tag_init_bool(result, tmp_a.data.bool & b->data.s32);
    case TAG_S16:
      return tag_init_bool(result, tmp_a.data.bool & b->data.s16);
    case TAG_S8:
      return tag_init_bool(result, tmp_a.data.bool & b->data.s8);
    case TAG_U8:
      return tag_init_bool(result, tmp_a.data.bool & b->data.u8);
    case TAG_U16:
      return tag_init_bool(result, tmp_a.data.bool & b->data.u16);
    case TAG_U32:
      return tag_init_bool(result, tmp_a.data.bool & b->data.u32);
    case TAG_U64:
      return tag_init_bool(result, tmp_a.data.bool & b->data.u64);
    case TAG_UW:
      return tag_init_bool(result, tmp_a.data.bool & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_CHARACTER:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_character(result, a->data.character &
                                (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_character(result, a->data.character & b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.character);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_character(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_character(result, a->data.character & b->data.sw);
    case TAG_S64:
      return tag_init_character(result, a->data.character & b->data.s64);
    case TAG_S32:
      return tag_init_character(result, a->data.character & b->data.s32);
    case TAG_S16:
      return tag_init_character(result, a->data.character & b->data.s16);
    case TAG_S8:
      return tag_init_character(result, a->data.character & b->data.s8);
    case TAG_U8:
      return tag_init_character(result, a->data.character & b->data.u8);
    case TAG_U16:
      return tag_init_character(result, a->data.character & b->data.u16);
    case TAG_U32:
      return tag_init_character(result, a->data.character & b->data.u32);
    case TAG_U64:
      return tag_init_character(result, a->data.character & b->data.u64);
    case TAG_UW:
      return tag_init_character(result, a->data.character & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_BOOL:
      integer_init_u8(&tmp, b->data.bool ? 1 : 0);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_CHARACTER:
      integer_init_u32(&tmp, b->data.character);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_INTEGER:
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &b->data.integer,
                   &result->data.integer);
      return result;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      result->type = TAG_INTEGER;
      integer_band(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_sw(result, a->data.sw &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_sw(result, a->data.sw & b->data.character);
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_sw(result, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_sw(result, a->data.sw & b->data.sw);
    case TAG_S64:
      return tag_init_sw(result, a->data.sw & b->data.s64);
    case TAG_S32:
      return tag_init_sw(result, a->data.sw & b->data.s32);
    case TAG_S16:
      return tag_init_sw(result, a->data.sw & b->data.s16);
    case TAG_S8:
      return tag_init_sw(result, a->data.sw & b->data.s8);
    case TAG_U8:
      return tag_init_sw(result, a->data.sw & b->data.u8);
    case TAG_U16:
      return tag_init_sw(result, a->data.sw & b->data.u16);
    case TAG_U32:
      return tag_init_sw(result, a->data.sw & b->data.u32);
    case TAG_U64:
      return tag_init_sw(result, a->data.sw & b->data.u64);
    case TAG_UW:
      return tag_init_sw(result, a->data.sw & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s64(result, a->data.s64 &
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s64(result, a->data.s64 & b->data.character);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_s64(result, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s64(result, a->data.s64 & b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, a->data.s64 & b->data.s64);
    case TAG_S32:
      return tag_init_s64(result, a->data.s64 & b->data.s32);
    case TAG_S16:
      return tag_init_s64(result, a->data.s64 & b->data.s16);
    case TAG_S8:
      return tag_init_s64(result, a->data.s64 & b->data.s8);
    case TAG_U8:
      return tag_init_s64(result, a->data.s64 & b->data.u8);
    case TAG_U16:
      return tag_init_s64(result, a->data.s64 & b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, a->data.s64 & b->data.u32);
    case TAG_U64:
      return tag_init_s64(result, a->data.s64 & b->data.u64);
    case TAG_UW:
      return tag_init_s64(result, a->data.s64 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s32(result, a->data.s32 &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s32(result, a->data.s32 & b->data.character);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_s32(result, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s32(result, a->data.s32 & b->data.sw);
    case TAG_S64:
      return tag_init_s32(result, a->data.s32 & b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, a->data.s32 & b->data.s32);
    case TAG_S16:
      return tag_init_s32(result, a->data.s32 & b->data.s16);
    case TAG_S8:
      return tag_init_s32(result, a->data.s32 & b->data.s8);
    case TAG_U8:
      return tag_init_s32(result, a->data.s32 & b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, a->data.s32 & b->data.u16);
    case TAG_U32:
      return tag_init_s32(result, a->data.s32 & b->data.u32);
    case TAG_U64:
      return tag_init_s32(result, a->data.s32 & b->data.u64);
    case TAG_UW:
      return tag_init_s32(result, a->data.s32 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s16(result, a->data.s16 &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s16(result, a->data.s16 & b->data.character);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_s16(result, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s16(result, a->data.s16 & b->data.sw);
    case TAG_S64:
      return tag_init_s16(result, a->data.s16 & b->data.s64);
    case TAG_S32:
      return tag_init_s16(result, a->data.s16 & b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, a->data.s16 & b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, a->data.s16 & b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, a->data.s16 & b->data.u8);
    case TAG_U16:
      return tag_init_s16(result, a->data.s16 & b->data.u16);
    case TAG_U32:
      return tag_init_s16(result, a->data.s16 & b->data.u32);
    case TAG_U64:
      return tag_init_s16(result, a->data.s16 & b->data.u64);
    case TAG_UW:
      return tag_init_s16(result, a->data.s16 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s8(result, a->data.s8 &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s8(result, a->data.s8 & b->data.character);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_s8(result, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s8(result, a->data.s8 & b->data.sw);
    case TAG_S64:
      return tag_init_s8(result, a->data.s8 & b->data.s64);
    case TAG_S32:
      return tag_init_s8(result, a->data.s8 & b->data.s32);
    case TAG_S16:
      return tag_init_s8(result, a->data.s8 & b->data.s16);
    case TAG_S8:
      return tag_init_s8(result, a->data.s8 & b->data.s8);
    case TAG_U8:
      return tag_init_s8(result, a->data.s8 & b->data.u8);
    case TAG_U16:
      return tag_init_s8(result, a->data.s8 & b->data.u16);
    case TAG_U32:
      return tag_init_s8(result, a->data.s8 & b->data.u32);
    case TAG_U64:
      return tag_init_s8(result, a->data.s8 & b->data.u64);
    case TAG_UW:
      return tag_init_s8(result, a->data.s8 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u8(result, a->data.u8 & b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u8(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u8(result, a->data.u8 & b->data.sw);
    case TAG_S64:
      return tag_init_u8(result, a->data.u8 & b->data.s64);
    case TAG_S32:
      return tag_init_u8(result, a->data.u8 & b->data.s32);
    case TAG_S16:
      return tag_init_u8(result, a->data.u8 & b->data.s16);
    case TAG_S8:
      return tag_init_u8(result, a->data.u8 & b->data.s8);
    case TAG_U8:
      return tag_init_u8(result, a->data.u8 & b->data.u8);
    case TAG_U16:
      return tag_init_u8(result, a->data.u8 & b->data.u16);
    case TAG_U32:
      return tag_init_u8(result, a->data.u8 & b->data.u32);
    case TAG_U64:
      return tag_init_u8(result, a->data.u8 & b->data.u64);
    case TAG_UW:
      return tag_init_u8(result, a->data.u8 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u16(result, a->data.u16 & b->data.character);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u16(result, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u16(result, a->data.u16 & b->data.sw);
    case TAG_S64:
      return tag_init_u16(result, a->data.u16 & b->data.s64);
    case TAG_S32:
      return tag_init_u16(result, a->data.u16 & b->data.s32);
    case TAG_S16:
      return tag_init_u16(result, a->data.u16 & b->data.s16);
    case TAG_S8:
      return tag_init_u16(result, a->data.u16 & b->data.s8);
    case TAG_U8:
      return tag_init_u16(result, a->data.u16 & b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, a->data.u16 & b->data.u16);
    case TAG_U32:
      return tag_init_u16(result, a->data.u16 & b->data.u32);
    case TAG_U64:
      return tag_init_u16(result, a->data.u16 & b->data.u64);
    case TAG_UW:
      return tag_init_u16(result, a->data.u16 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 &
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u32 & b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u32(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u32(result, a->data.u32 & b->data.sw);
    case TAG_S64:
      return tag_init_u32(result, a->data.u32 & b->data.s64);
    case TAG_S32:
      return tag_init_u32(result, a->data.u32 & b->data.s32);
    case TAG_S16:
      return tag_init_u32(result, a->data.u32 & b->data.s16);
    case TAG_S8:
      return tag_init_u32(result, a->data.u32 & b->data.s8);
    case TAG_U8:
      return tag_init_u32(result, a->data.u32 & b->data.u8);
    case TAG_U16:
      return tag_init_u32(result, a->data.u32 & b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, a->data.u32 & b->data.u32);
    case TAG_U64:
      return tag_init_u32(result, a->data.u32 & b->data.u64);
    case TAG_UW:
      return tag_init_u32(result, a->data.u32 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u64(result, a->data.u64 & b->data.character);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_u64(result, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u64(result, a->data.u64 & b->data.sw);
    case TAG_S64:
      return tag_init_u64(result, a->data.u64 & b->data.s64);
    case TAG_S32:
      return tag_init_u64(result, a->data.u64 & b->data.s32);
    case TAG_S16:
      return tag_init_u64(result, a->data.u64 & b->data.s16);
    case TAG_S8:
      return tag_init_u64(result, a->data.u64 & b->data.s8);
    case TAG_U8:
      return tag_init_u64(result, a->data.u64 & b->data.u8);
    case TAG_U16:
      return tag_init_u64(result, a->data.u64 & b->data.u16);
    case TAG_U32:
      return tag_init_u64(result, a->data.u64 & b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, a->data.u64 & b->data.u64);
    case TAG_UW:
      return tag_init_u64(result, a->data.u64 & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw &
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_uw(result, a->data.uw & b->data.character);
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      integer_band(&tmp, &b->data.integer, &tmp2);
      tag_init_uw(result, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_uw(result, a->data.uw & b->data.sw);
    case TAG_S64:
      return tag_init_uw(result, a->data.uw & b->data.s64);
    case TAG_S32:
      return tag_init_uw(result, a->data.uw & b->data.s32);
    case TAG_S16:
      return tag_init_uw(result, a->data.uw & b->data.s16);
    case TAG_S8:
      return tag_init_uw(result, a->data.uw & b->data.s8);
    case TAG_U8:
      return tag_init_uw(result, a->data.uw & b->data.u8);
    case TAG_U16:
      return tag_init_uw(result, a->data.uw & b->data.u16);
    case TAG_U32:
      return tag_init_uw(result, a->data.uw & b->data.u32);
    case TAG_U64:
      return tag_init_uw(result, a->data.uw & b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, a->data.uw & b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto error;
  }
 error:
  warnx("tag_band: invalid tag type: %s & %s",
        tag_type_to_string(a->type),
        tag_type_to_string(b->type));
  return NULL;
}

s_tag * tag_bnot (const s_tag *tag, s_tag *result)
{
  switch (tag->type) {
  case TAG_BOOL:
    return tag_init_u8(result, ~(tag->data.bool ? 1 : 0));
  case TAG_CHARACTER:
    return tag_init_u32(result, ~tag->data.character);
  case TAG_INTEGER:
    result->type = TAG_INTEGER;
    integer_bnot(&tag->data.integer, &result->data.integer);
    return result;
  case TAG_SW:
    return tag_init_sw(result, ~tag->data.sw);
  case TAG_S64:
    return tag_init_s64(result, ~tag->data.s64);
  case TAG_S32:
    return tag_init_s32(result, ~tag->data.s32);
  case TAG_S16:
    return tag_init_s16(result, ~tag->data.s16);
  case TAG_S8:
    return tag_init_s8(result, ~tag->data.s8);
  case TAG_U8:
    return tag_init_u8(result, ~tag->data.u8);
  case TAG_U16:
    return tag_init_u16(result, ~tag->data.u16);
  case TAG_U32:
    return tag_init_u32(result, ~tag->data.u32);
  case TAG_U64:
    return tag_init_u64(result, ~tag->data.u64);
  case TAG_UW:
    return tag_init_uw(result, ~tag->data.uw);
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
  case TAG_VOID:
    warnx("tag_bnot: invalid tag type: %s",
          tag_type_to_string(tag->type));
  }
  return NULL;
}

s_tag * tag_bool (s_tag *tag, bool b)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_bool(tag, b);
}

s_tag * tag_bor (const s_tag *a, const s_tag *b, s_tag *result)
{
  s_integer tmp;
  s_integer tmp2;
  s_tag tmp_a;
  switch (a->type) {
  case TAG_BOOL:
    tmp_a.data.bool = a->data.bool ? 1 : 0;
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_bool(result, tmp_a.data.bool |
                           (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_bool(result, tmp_a.data.bool | b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, tmp_a.data.bool);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_bool(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_bool(result, tmp_a.data.bool | b->data.sw);
    case TAG_S64:
      return tag_init_bool(result, tmp_a.data.bool | b->data.s64);
    case TAG_S32:
      return tag_init_bool(result, tmp_a.data.bool | b->data.s32);
    case TAG_S16:
      return tag_init_bool(result, tmp_a.data.bool | b->data.s16);
    case TAG_S8:
      return tag_init_bool(result, tmp_a.data.bool | b->data.s8);
    case TAG_U8:
      return tag_init_bool(result, tmp_a.data.bool | b->data.u8);
    case TAG_U16:
      return tag_init_bool(result, tmp_a.data.bool | b->data.u16);
    case TAG_U32:
      return tag_init_bool(result, tmp_a.data.bool | b->data.u32);
    case TAG_U64:
      return tag_init_bool(result, tmp_a.data.bool | b->data.u64);
    case TAG_UW:
      return tag_init_bool(result, tmp_a.data.bool | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_CHARACTER:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_character(result, a->data.character |
                                (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_character(result, a->data.character | b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.character);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_character(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_character(result, a->data.character | b->data.sw);
    case TAG_S64:
      return tag_init_character(result, a->data.character | b->data.s64);
    case TAG_S32:
      return tag_init_character(result, a->data.character | b->data.s32);
    case TAG_S16:
      return tag_init_character(result, a->data.character | b->data.s16);
    case TAG_S8:
      return tag_init_character(result, a->data.character | b->data.s8);
    case TAG_U8:
      return tag_init_character(result, a->data.character | b->data.u8);
    case TAG_U16:
      return tag_init_character(result, a->data.character | b->data.u16);
    case TAG_U32:
      return tag_init_character(result, a->data.character | b->data.u32);
    case TAG_U64:
      return tag_init_character(result, a->data.character | b->data.u64);
    case TAG_UW:
      return tag_init_character(result, a->data.character | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_BOOL:
      integer_init_u8(&tmp, b->data.bool ? 1 : 0);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_CHARACTER:
      integer_init_u32(&tmp, b->data.character);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_INTEGER:
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &b->data.integer,
                   &result->data.integer);
      return result;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      result->type = TAG_INTEGER;
      integer_bor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_sw(result, a->data.sw |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_sw(result, a->data.sw | b->data.character);
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_sw(result, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_sw(result, a->data.sw | b->data.sw);
    case TAG_S64:
      return tag_init_sw(result, a->data.sw | b->data.s64);
    case TAG_S32:
      return tag_init_sw(result, a->data.sw | b->data.s32);
    case TAG_S16:
      return tag_init_sw(result, a->data.sw | b->data.s16);
    case TAG_S8:
      return tag_init_sw(result, a->data.sw | b->data.s8);
    case TAG_U8:
      return tag_init_sw(result, a->data.sw | b->data.u8);
    case TAG_U16:
      return tag_init_sw(result, a->data.sw | b->data.u16);
    case TAG_U32:
      return tag_init_sw(result, a->data.sw | b->data.u32);
    case TAG_U64:
      return tag_init_sw(result, a->data.sw | b->data.u64);
    case TAG_UW:
      return tag_init_sw(result, a->data.sw | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s64(result, a->data.s64 |
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s64(result, a->data.s64 | b->data.character);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_s64(result, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s64(result, a->data.s64 | b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, a->data.s64 | b->data.s64);
    case TAG_S32:
      return tag_init_s64(result, a->data.s64 | b->data.s32);
    case TAG_S16:
      return tag_init_s64(result, a->data.s64 | b->data.s16);
    case TAG_S8:
      return tag_init_s64(result, a->data.s64 | b->data.s8);
    case TAG_U8:
      return tag_init_s64(result, a->data.s64 | b->data.u8);
    case TAG_U16:
      return tag_init_s64(result, a->data.s64 | b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, a->data.s64 | b->data.u32);
    case TAG_U64:
      return tag_init_s64(result, a->data.s64 | b->data.u64);
    case TAG_UW:
      return tag_init_s64(result, a->data.s64 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s32(result, a->data.s32 |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s32(result, a->data.s32 | b->data.character);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_s32(result, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s32(result, a->data.s32 | b->data.sw);
    case TAG_S64:
      return tag_init_s32(result, a->data.s32 | b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, a->data.s32 | b->data.s32);
    case TAG_S16:
      return tag_init_s32(result, a->data.s32 | b->data.s16);
    case TAG_S8:
      return tag_init_s32(result, a->data.s32 | b->data.s8);
    case TAG_U8:
      return tag_init_s32(result, a->data.s32 | b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, a->data.s32 | b->data.u16);
    case TAG_U32:
      return tag_init_s32(result, a->data.s32 | b->data.u32);
    case TAG_U64:
      return tag_init_s32(result, a->data.s32 | b->data.u64);
    case TAG_UW:
      return tag_init_s32(result, a->data.s32 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s16(result, a->data.s16 |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s16(result, a->data.s16 | b->data.character);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_s16(result, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s16(result, a->data.s16 | b->data.sw);
    case TAG_S64:
      return tag_init_s16(result, a->data.s16 | b->data.s64);
    case TAG_S32:
      return tag_init_s16(result, a->data.s16 | b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, a->data.s16 | b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, a->data.s16 | b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, a->data.s16 | b->data.u8);
    case TAG_U16:
      return tag_init_s16(result, a->data.s16 | b->data.u16);
    case TAG_U32:
      return tag_init_s16(result, a->data.s16 | b->data.u32);
    case TAG_U64:
      return tag_init_s16(result, a->data.s16 | b->data.u64);
    case TAG_UW:
      return tag_init_s16(result, a->data.s16 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s8(result, a->data.s8 |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s8(result, a->data.s8 | b->data.character);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_s8(result, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s8(result, a->data.s8 | b->data.sw);
    case TAG_S64:
      return tag_init_s8(result, a->data.s8 | b->data.s64);
    case TAG_S32:
      return tag_init_s8(result, a->data.s8 | b->data.s32);
    case TAG_S16:
      return tag_init_s8(result, a->data.s8 | b->data.s16);
    case TAG_S8:
      return tag_init_s8(result, a->data.s8 | b->data.s8);
    case TAG_U8:
      return tag_init_s8(result, a->data.s8 | b->data.u8);
    case TAG_U16:
      return tag_init_s8(result, a->data.s8 | b->data.u16);
    case TAG_U32:
      return tag_init_s8(result, a->data.s8 | b->data.u32);
    case TAG_U64:
      return tag_init_s8(result, a->data.s8 | b->data.u64);
    case TAG_UW:
      return tag_init_s8(result, a->data.s8 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u8(result, a->data.u8 | b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_u8(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u8(result, a->data.u8 | b->data.sw);
    case TAG_S64:
      return tag_init_u8(result, a->data.u8 | b->data.s64);
    case TAG_S32:
      return tag_init_u8(result, a->data.u8 | b->data.s32);
    case TAG_S16:
      return tag_init_u8(result, a->data.u8 | b->data.s16);
    case TAG_S8:
      return tag_init_u8(result, a->data.u8 | b->data.s8);
    case TAG_U8:
      return tag_init_u8(result, a->data.u8 | b->data.u8);
    case TAG_U16:
      return tag_init_u8(result, a->data.u8 | b->data.u16);
    case TAG_U32:
      return tag_init_u8(result, a->data.u8 | b->data.u32);
    case TAG_U64:
      return tag_init_u8(result, a->data.u8 | b->data.u64);
    case TAG_UW:
      return tag_init_u8(result, a->data.u8 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u16(result, a->data.u16 | b->data.character);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_u16(result, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u16(result, a->data.u16 | b->data.sw);
    case TAG_S64:
      return tag_init_u16(result, a->data.u16 | b->data.s64);
    case TAG_S32:
      return tag_init_u16(result, a->data.u16 | b->data.s32);
    case TAG_S16:
      return tag_init_u16(result, a->data.u16 | b->data.s16);
    case TAG_S8:
      return tag_init_u16(result, a->data.u16 | b->data.s8);
    case TAG_U8:
      return tag_init_u16(result, a->data.u16 | b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, a->data.u16 | b->data.u16);
    case TAG_U32:
      return tag_init_u16(result, a->data.u16 | b->data.u32);
    case TAG_U64:
      return tag_init_u16(result, a->data.u16 | b->data.u64);
    case TAG_UW:
      return tag_init_u16(result, a->data.u16 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 |
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u32 | b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_u32(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u32(result, a->data.u32 | b->data.sw);
    case TAG_S64:
      return tag_init_u32(result, a->data.u32 | b->data.s64);
    case TAG_S32:
      return tag_init_u32(result, a->data.u32 | b->data.s32);
    case TAG_S16:
      return tag_init_u32(result, a->data.u32 | b->data.s16);
    case TAG_S8:
      return tag_init_u32(result, a->data.u32 | b->data.s8);
    case TAG_U8:
      return tag_init_u32(result, a->data.u32 | b->data.u8);
    case TAG_U16:
      return tag_init_u32(result, a->data.u32 | b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, a->data.u32 | b->data.u32);
    case TAG_U64:
      return tag_init_u32(result, a->data.u32 | b->data.u64);
    case TAG_UW:
      return tag_init_u32(result, a->data.u32 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u64(result, a->data.u64 | b->data.character);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_u64(result, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u64(result, a->data.u64 | b->data.sw);
    case TAG_S64:
      return tag_init_u64(result, a->data.u64 | b->data.s64);
    case TAG_S32:
      return tag_init_u64(result, a->data.u64 | b->data.s32);
    case TAG_S16:
      return tag_init_u64(result, a->data.u64 | b->data.s16);
    case TAG_S8:
      return tag_init_u64(result, a->data.u64 | b->data.s8);
    case TAG_U8:
      return tag_init_u64(result, a->data.u64 | b->data.u8);
    case TAG_U16:
      return tag_init_u64(result, a->data.u64 | b->data.u16);
    case TAG_U32:
      return tag_init_u64(result, a->data.u64 | b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, a->data.u64 | b->data.u64);
    case TAG_UW:
      return tag_init_u64(result, a->data.u64 | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw |
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_uw(result, a->data.uw | b->data.character);
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      integer_bor(&tmp, &b->data.integer, &tmp2);
      tag_init_uw(result, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_uw(result, a->data.uw | b->data.sw);
    case TAG_S64:
      return tag_init_uw(result, a->data.uw | b->data.s64);
    case TAG_S32:
      return tag_init_uw(result, a->data.uw | b->data.s32);
    case TAG_S16:
      return tag_init_uw(result, a->data.uw | b->data.s16);
    case TAG_S8:
      return tag_init_uw(result, a->data.uw | b->data.s8);
    case TAG_U8:
      return tag_init_uw(result, a->data.uw | b->data.u8);
    case TAG_U16:
      return tag_init_uw(result, a->data.uw | b->data.u16);
    case TAG_U32:
      return tag_init_uw(result, a->data.uw | b->data.u32);
    case TAG_U64:
      return tag_init_uw(result, a->data.uw | b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, a->data.uw | b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto error;
  }
 error:
  warnx("tag_bor: invalid tag type: %s | %s",
        tag_type_to_string(a->type),
        tag_type_to_string(b->type));
  return NULL;
}

s_tag * tag_bxor (const s_tag *a, const s_tag *b, s_tag *result)
{
  s_integer tmp;
  s_integer tmp2;
  s_tag tmp_a;
  switch (a->type) {
  case TAG_BOOL:
    tmp_a.data.bool = a->data.bool ? 1 : 0;
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_bool(result, tmp_a.data.bool ^
                           (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, tmp_a.data.bool);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_bool(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.sw);
    case TAG_S64:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.s64);
    case TAG_S32:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.s32);
    case TAG_S16:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.s16);
    case TAG_S8:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.s8);
    case TAG_U8:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.u8);
    case TAG_U16:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.u16);
    case TAG_U32:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.u32);
    case TAG_U64:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.u64);
    case TAG_UW:
      return tag_init_bool(result, tmp_a.data.bool ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_CHARACTER:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_character(result, a->data.character ^
                                (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_character(result, a->data.character ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.character);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_character(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_character(result, a->data.character ^ b->data.sw);
    case TAG_S64:
      return tag_init_character(result, a->data.character ^ b->data.s64);
    case TAG_S32:
      return tag_init_character(result, a->data.character ^ b->data.s32);
    case TAG_S16:
      return tag_init_character(result, a->data.character ^ b->data.s16);
    case TAG_S8:
      return tag_init_character(result, a->data.character ^ b->data.s8);
    case TAG_U8:
      return tag_init_character(result, a->data.character ^ b->data.u8);
    case TAG_U16:
      return tag_init_character(result, a->data.character ^ b->data.u16);
    case TAG_U32:
      return tag_init_character(result, a->data.character ^ b->data.u32);
    case TAG_U64:
      return tag_init_character(result, a->data.character ^ b->data.u64);
    case TAG_UW:
      return tag_init_character(result, a->data.character ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_BOOL:
      integer_init_u8(&tmp, b->data.bool ? 1 : 0);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    case TAG_CHARACTER:
      integer_init_u32(&tmp, b->data.character);
      result->type = TAG_INTEGER;
      integer_bxor(&a->data.integer, &tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_sw(result, a->data.sw ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_sw(result, a->data.sw ^ b->data.character);
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_sw(result, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_sw(result, a->data.sw ^ b->data.sw);
    case TAG_S64:
      return tag_init_sw(result, a->data.sw ^ b->data.s64);
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
      return tag_init_sw(result, a->data.sw ^ b->data.u32);
    case TAG_U64:
      return tag_init_sw(result, a->data.sw ^ b->data.u64);
    case TAG_UW:
      return tag_init_sw(result, a->data.sw ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s64(result, a->data.s64 ^
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s64(result, a->data.s64 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_s64(result, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
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
      return tag_init_s64(result, a->data.s64 ^ b->data.u64);
    case TAG_UW:
      return tag_init_s64(result, a->data.s64 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s32(result, a->data.s32 ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s32(result, a->data.s32 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_s32(result, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s32(result, a->data.s32 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s32(result, a->data.s32 ^ b->data.s64);
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
      return tag_init_s32(result, a->data.s32 ^ b->data.u32);
    case TAG_U64:
      return tag_init_s32(result, a->data.s32 ^ b->data.u64);
    case TAG_UW:
      return tag_init_s32(result, a->data.s32 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s16(result, a->data.s16 ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s16(result, a->data.s16 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_s16(result, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s16(result, a->data.s16 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s16(result, a->data.s16 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s16(result, a->data.s16 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, a->data.s16 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, a->data.s16 ^ b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, a->data.s16 ^ b->data.u8);
    case TAG_U16:
      return tag_init_s16(result, a->data.s16 ^ b->data.u16);
    case TAG_U32:
      return tag_init_s16(result, a->data.s16 ^ b->data.u32);
    case TAG_U64:
      return tag_init_s16(result, a->data.s16 ^ b->data.u64);
    case TAG_UW:
      return tag_init_s16(result, a->data.s16 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s8(result, a->data.s8 ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s8(result, a->data.s8 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_s8(result, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s8(result, a->data.s8 ^ b->data.sw);
    case TAG_S64:
      return tag_init_s8(result, a->data.s8 ^ b->data.s64);
    case TAG_S32:
      return tag_init_s8(result, a->data.s8 ^ b->data.s32);
    case TAG_S16:
      return tag_init_s8(result, a->data.s8 ^ b->data.s16);
    case TAG_S8:
      return tag_init_s8(result, a->data.s8 ^ b->data.s8);
    case TAG_U8:
      return tag_init_s8(result, a->data.s8 ^ b->data.u8);
    case TAG_U16:
      return tag_init_s8(result, a->data.s8 ^ b->data.u16);
    case TAG_U32:
      return tag_init_s8(result, a->data.s8 ^ b->data.u32);
    case TAG_U64:
      return tag_init_s8(result, a->data.s8 ^ b->data.u64);
    case TAG_UW:
      return tag_init_s8(result, a->data.s8 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u8(result, a->data.u8 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_u8(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u8(result, a->data.u8 ^ b->data.sw);
    case TAG_S64:
      return tag_init_u8(result, a->data.u8 ^ b->data.s64);
    case TAG_S32:
      return tag_init_u8(result, a->data.u8 ^ b->data.s32);
    case TAG_S16:
      return tag_init_u8(result, a->data.u8 ^ b->data.s16);
    case TAG_S8:
      return tag_init_u8(result, a->data.u8 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u8(result, a->data.u8 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u8(result, a->data.u8 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u8(result, a->data.u8 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u8(result, a->data.u8 ^ b->data.u64);
    case TAG_UW:
      return tag_init_u8(result, a->data.u8 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u16(result, a->data.u16 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_u16(result, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u16(result, a->data.u16 ^ b->data.sw);
    case TAG_S64:
      return tag_init_u16(result, a->data.u16 ^ b->data.s64);
    case TAG_S32:
      return tag_init_u16(result, a->data.u16 ^ b->data.s32);
    case TAG_S16:
      return tag_init_u16(result, a->data.u16 ^ b->data.s16);
    case TAG_S8:
      return tag_init_u16(result, a->data.u16 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u16(result, a->data.u16 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, a->data.u16 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u16(result, a->data.u16 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u16(result, a->data.u16 ^ b->data.u64);
    case TAG_UW:
      return tag_init_u16(result, a->data.u16 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 ^
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u32 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_u32(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u32(result, a->data.u32 ^ b->data.sw);
    case TAG_S64:
      return tag_init_u32(result, a->data.u32 ^ b->data.s64);
    case TAG_S32:
      return tag_init_u32(result, a->data.u32 ^ b->data.s32);
    case TAG_S16:
      return tag_init_u32(result, a->data.u32 ^ b->data.s16);
    case TAG_S8:
      return tag_init_u32(result, a->data.u32 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u32(result, a->data.u32 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u32(result, a->data.u32 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, a->data.u32 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u32(result, a->data.u32 ^ b->data.u64);
    case TAG_UW:
      return tag_init_u32(result, a->data.u32 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 ^
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u64(result, a->data.u64 ^ b->data.character);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      integer_bxor(&tmp, &b->data.integer, &tmp2);
      tag_init_u64(result, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u64(result, a->data.u64 ^ b->data.sw);
    case TAG_S64:
      return tag_init_u64(result, a->data.u64 ^ b->data.s64);
    case TAG_S32:
      return tag_init_u64(result, a->data.u64 ^ b->data.s32);
    case TAG_S16:
      return tag_init_u64(result, a->data.u64 ^ b->data.s16);
    case TAG_S8:
      return tag_init_u64(result, a->data.u64 ^ b->data.s8);
    case TAG_U8:
      return tag_init_u64(result, a->data.u64 ^ b->data.u8);
    case TAG_U16:
      return tag_init_u64(result, a->data.u64 ^ b->data.u16);
    case TAG_U32:
      return tag_init_u64(result, a->data.u64 ^ b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, a->data.u64 ^ b->data.u64);
    case TAG_UW:
      return tag_init_u64(result, a->data.u64 ^ b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw ^
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto error;
  }
 error:
  warnx("tag_bxor: invalid tag type: %s ^ %s",
        tag_type_to_string(a->type),
        tag_type_to_string(b->type));
  return NULL;
}

s_tag * tag_cast_integer_to_s8 (s_tag *tag)
{
  s8 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s8(&tag->data.integer);
  return tag_s8(tag, i);
}

s_tag * tag_cast_integer_to_s16 (s_tag *tag)
{
  s16 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s16(&tag->data.integer);
  return tag_s16(tag, i);
}

s_tag * tag_cast_integer_to_s32 (s_tag *tag)
{
  s32 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s32(&tag->data.integer);
  return tag_s32(tag, i);
}

s_tag * tag_cast_integer_to_s64 (s_tag *tag)
{
  s64 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s64(&tag->data.integer);
  return tag_s64(tag, i);
}

s_tag * tag_cast_integer_to_u8 (s_tag *tag)
{
  u8 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u8(&tag->data.integer);
  return tag_u8(tag, i);
}

s_tag * tag_cast_integer_to_u16 (s_tag *tag)
{
  u16 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u16(&tag->data.integer);
  return tag_u16(tag, i);
}

s_tag * tag_cast_integer_to_u32 (s_tag *tag)
{
  u32 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u32(&tag->data.integer);
  return tag_u32(tag, i);
}

s_tag * tag_cast_integer_to_u64 (s_tag *tag)
{
  u64 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u64(&tag->data.integer);
  return tag_u64(tag, i);
}

s_tag * tag_character (s_tag *tag, character x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_character(tag, x);
}

void tag_clean (s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:   array_clean(&tag->data.array);     break;
  case TAG_CALL:    call_clean(&tag->data.call);       break;
  case TAG_CFN:     cfn_clean(&tag->data.cfn);         break;
  case TAG_FN:      fn_clean(&tag->data.fn);           break;
  case TAG_INTEGER: integer_clean(&tag->data.integer); break;
  case TAG_LIST:    list_delete_all(tag->data.list);   break;
  case TAG_QUOTE:   quote_clean(&tag->data.quote);     break;
  case TAG_STR:     str_clean(&tag->data.str);         break;
  case TAG_TUPLE:   tuple_clean(&tag->data.tuple);     break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:
  case TAG_VOID:
    break;
  }
}

s_tag * tag_brackets (s_tag *tag, const s_tag *address,
                      s_tag *dest)
{
  assert(tag);
  assert(address);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    switch (address->type) {
    case TAG_ARRAY:
      return array_data_tag(tag, address, dest);
    default:
      break;
    }
  default:
    break;
  }
  warnx("tag_brackets: invalid arguments");
  return NULL;
}

s_tag * tag_copy (const s_tag *src, s_tag *dest)
{
  assert(src);
  assert(dest);
  switch (src->type) {
  case TAG_VAR:
    tag_init_var(dest);
    break;
  case TAG_VOID:
    break;
  case TAG_ARRAY:
    array_copy(&src->data.array, &dest->data.array);
    break;
  case TAG_CALL:
    call_copy(&src->data.call, &dest->data.call);
    break;
  case TAG_CFN:
    cfn_copy(&src->data.cfn, &dest->data.cfn);
    break;
  case TAG_FN:
    fn_copy(&src->data.fn, &dest->data.fn);
    break;
  case TAG_INTEGER:
    integer_copy(&src->data.integer, &dest->data.integer);
    break;
  case TAG_LIST:
    list_copy((const s_list **) &src->data.list, &dest->data.list);
    break;
  case TAG_QUOTE:
    quote_copy(&src->data.quote, &dest->data.quote);
    break;
  case TAG_STR:
    str_copy(&src->data.str, &dest->data.str);
    break;
  case TAG_TUPLE:
    tuple_copy(&src->data.tuple, &dest->data.tuple);
    break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    dest->data = src->data;
  }
  dest->type = src->type;
  return dest;
}

void tag_delete (s_tag *tag)
{
  tag_clean(tag);
  free(tag);
}

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
      integer_init_f32(&tmp, a->data.f32);
      integer_div(&tmp, &b->data.integer, &tmp);
      tag_init_integer(dest, &tmp);
      integer_clean(&tmp);
      return dest;
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
      integer_init_f64(&tmp, a->data.f64);
      integer_div(&tmp, &b->data.integer, &tmp);
      tag_init_integer(dest, &tmp);
      integer_clean(&tmp);
      return dest;
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
  assert(! "tag_div: invalid tag type");
  warnx("tag_div: invalid tag type: %s / %s",
        tag_type_to_string(a->type),
        tag_type_to_string(b->type));
  return NULL;
}

bool tag_eq (const s_tag *a, const s_tag *b)
{
  return compare_tag(a, b) == 0;
}

s_tag * tag_equal (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  if (! env_eval_equal_tag (&g_c3_env, a, b, dest))
    return NULL;
  return dest;
}

s_tag * tag_f32 (s_tag *tag, f32 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_f32(tag, x);
}

s_tag * tag_f64 (s_tag *tag, f64 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_f64(tag, x);
}

bool tag_gt (const s_tag *a, const s_tag *b)
{
  return compare_tag(a, b) > 0;
}

bool tag_gte (const s_tag *a, const s_tag *b)
{
  return compare_tag(a, b) >= 0;
}

u64 tag_hash_u64 (const s_tag *tag)
{
  t_hash hash;
  assert(tag);
  hash_init(&hash);
  hash_update_tag(&hash, tag);
  return hash_to_u64(&hash);
}

uw tag_hash_uw (const s_tag *tag)
{
  t_hash hash;
  assert(tag);
  hash_init(&hash);
  hash_update_tag(&hash, tag);
  return hash_to_uw(&hash);
}

s_tag * tag_ident (s_tag *tag, const s_ident *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_ident(tag, x);
}

s_tag * tag_ident_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_ident_1(tag, p);
}

bool tag_ident_is_bound (const s_tag *tag)
{
  return env_tag_ident_is_bound(&g_c3_env, tag, &g_c3_env.facts);
}

s_tag * tag_init (s_tag *tag)
{
  bzero(tag, sizeof(s_tag));
  return tag;
}

s_tag * tag_init_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  tag_init_void(tag);
  if (! p)
    return tag;
  buf_init_1(&buf, p);
  if (buf_parse_tag(&buf, tag) != (sw) strlen(p)) {
    assert(! "invalid tag");
    errx(1, "invalid tag");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_array (s_tag *tag, const s_array *a)
{
  assert(tag);
  assert(a);
  tag->type = TAG_ARRAY;
  array_copy(a, &tag->data.array);
  return tag;
}

s_tag * tag_init_bool (s_tag *tag, bool b)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_BOOL;
  tag->data.bool = b;
  return tag;
}

s_tag * tag_init_call (s_tag *tag, const s_call *call)
{
  assert(tag);
  assert(call);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_CALL;
  tag->data.call = *call;
  return tag;
}

s_tag * tag_init_character (s_tag *tag, character c)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_CHARACTER;
  tag->data.character = c;
  return tag;
}

s_tag * tag_init_f32 (s_tag *tag, f32 x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_F32;
  tag->data.f32 = x;
  return tag;
}

s_tag * tag_init_f64 (s_tag *tag, f64 x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_F64;
  tag->data.f64 = x;
  return tag;
}

s_tag * tag_init_ident (s_tag *tag, const s_ident *x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_IDENT;
  tag->data.ident = *x;
  return tag;
}

s_tag * tag_init_ident_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_IDENT;
  ident_init_1(&tag->data.ident, p);
  return tag;
}

s_tag * tag_init_integer (s_tag *tag, const s_integer *i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init(&tag->data.integer);
  integer_copy(i, &tag->data.integer);
  return tag;
}

s_tag * tag_init_integer_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  buf_init_1(&buf, p);
  if (buf_parse_integer(&buf, &tag->data.integer) != (sw) strlen(p)) {
    assert(! "tag_init_integer_1: invalid integer");
    errx(1, "tag_init_integer_1: invalid integer");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_integer_s64 (s_tag *tag, s64 s)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_s64(&tag->data.integer, s);
  return tag;
}

s_tag * tag_init_integer_u64 (s_tag *tag, u64 u)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_u64(&tag->data.integer, u);
  return tag;
}

s_tag * tag_init_integer_zero (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_zero(&tag->data.integer);
  return tag;
}

s_tag * tag_init_list (s_tag *tag, s_list *list)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_LIST;
  tag->data.list = list;
  return tag;
}

s_tag * tag_init_list_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_LIST;
  buf_init_1(&buf, p);
  if (buf_parse_list(&buf, &tag->data.list) != (sw) strlen(p)) {
    assert(! "tag_init_list_1: invalid list");
    errx(1, "tag_init_list_1: invalid list");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_s8 (s_tag *tag, s8 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S8;
  tag->data.s8 = i;
  return tag;
}

s_tag * tag_init_s16 (s_tag *tag, s16 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S16;
  tag->data.s16 = i;
  return tag;
}

s_tag * tag_init_s32 (s_tag *tag, s32 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S32;
  tag->data.s32 = i;
  return tag;
}

s_tag * tag_init_s64 (s_tag *tag, s64 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S64;
  tag->data.s64 = i;
  return tag;
}

s_tag * tag_init_sw (s_tag *tag, sw i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SW;
  tag->data.sw = i;
  return tag;
}

s_tag * tag_init_str (s_tag *tag, s8 *free, uw size, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_STR;
  str_init(&tag->data.str, free, size, p);
  return tag;
}

s_tag * tag_init_str_1 (s_tag *tag, s8 *free, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_STR;
  str_init_1(&tag->data.str, free, p);
  return tag;
}

s_tag * tag_init_sym (s_tag *tag, const s_sym *p)
{
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SYM;
  tag->data.sym = p;
  return tag;
}

s_tag * tag_init_sym_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SYM;
  buf_init_1(&buf, p);
  if (buf_parse_sym(&buf, &tag->data.sym) != (sw) strlen(p)) {
    assert(! "tag_init_sym_1: invalid symbol");
    errx(1, "tag_init_sym_1: invalid symbol");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_time (s_tag *tag)
{
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return timespec_to_tag(&time, tag);
}

s_tag * tag_init_tuple (s_tag *tag, uw count)
{
  assert(tag);
  assert(count);
  tag->type = TAG_TUPLE;
  tuple_init(&tag->data.tuple, count);
  return tag;
}

s_tag * tag_init_u8 (s_tag *tag, u8 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U8;
  tag->data.u8 = i;
  return tag;
}

s_tag * tag_init_u16 (s_tag *tag, u16 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U16;
  tag->data.u16 = i;
  return tag;
}

s_tag * tag_init_u32 (s_tag *tag, u32 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U32;
  tag->data.u32 = i;
  return tag;
}

s_tag * tag_init_u64 (s_tag *tag, u64 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U64;
  tag->data.u64 = i;
  return tag;
}

s_tag * tag_init_uw (s_tag *tag, uw i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_UW;
  tag->data.uw = i;
  return tag;
}

s_tag * tag_init_var (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_VAR;
  return tag;
}

s_tag * tag_init_void (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_VOID;
  return tag;
}

s_str * tag_inspect (const s_tag *tag, s_str *dest)
{
  s_buf buf;
  sw size;
  if ((size = buf_inspect_tag_size(tag)) < 0) {
    assert(! "tag_inspect: size error");
    errx(1, "tag_inspect: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_tag(&buf, tag) != size) {
    assert(! "tag_inspect: inspect error");
    errx(1, "tag_inspect: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_tag * tag_integer (s_tag *tag, const s_integer *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_integer(tag, x);
}

s_tag * tag_integer_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_integer_1(tag, p);
}

s_tag * tag_integer_reduce (s_tag *tag)
{
  uw bytes;
  e_bool negative;
  assert(tag);
  switch (tag->type) {
  case TAG_INTEGER:
    bytes = integer_bytes(&tag->data.integer);
    if (bytes > 8)
      break;
    negative = integer_is_negative(&tag->data.integer);
    if (bytes > 4) {
      if (negative)
        return tag_cast_integer_to_s64(tag);
      return tag_cast_integer_to_u64(tag);
    }
    if (bytes > 2) {
      if (negative)
        return tag_cast_integer_to_s32(tag);
      return tag_cast_integer_to_u32(tag);
    }
    if (bytes > 1) {
      if (negative)
        return tag_cast_integer_to_s16(tag);
      return tag_cast_integer_to_u16(tag);
    }
    if (negative)
      return tag_cast_integer_to_s8(tag);
    return tag_cast_integer_to_u8(tag);
  default: ;
  }
  return tag;
}

bool tag_is_bound_var (const s_tag *tag)
{
  return (tag &&
          tag->type != TAG_VAR);
}

bool tag_is_number (const s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
  case TAG_INTEGER:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
    return true;
  default: ;
  }
  return false;
}

bool tag_is_unbound_var (const s_tag *tag)
{
  return (tag && tag->type == TAG_VAR);
}

s_tag * tag_list (s_tag *tag, s_list *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list(tag, x);
}

s_tag * tag_list_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list_1(tag, p);
}

bool tag_lt (const s_tag *a, const s_tag *b)
{
  return compare_tag(a, b) < 0;
}

bool tag_lte (const s_tag *a, const s_tag *b)
{
  return compare_tag(a, b) <= 0;
}

s_tag * tag_mod (const s_tag *a, const s_tag *b, s_tag *dest)
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
      return tag_init_f32(dest, fmodf(a->data.f32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.f32, b->data.f64));
    case TAG_INTEGER:
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     integer_to_f64(&b->data.integer)));
    case TAG_S8:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.s8));
    case TAG_S16:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.s16));
    case TAG_S32:
      return tag_init_f64(dest, fmod((f64) a->data.f32, (f64) b->data.s32));
    case TAG_S64:
      return tag_init_f64(dest, fmod((f64) a->data.f32, (f64) b->data.s64));
    case TAG_SW:
      return tag_init_f64(dest, fmod((f64) a->data.f32, (f64) b->data.sw));
    case TAG_U8:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.u8));
    case TAG_U16:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.u16));
    case TAG_U32:
      return tag_init_f64(dest, fmod((f64) a->data.f32, (f64) b->data.u32));
    case TAG_U64:
      return tag_init_f64(dest, fmod((f64) a->data.f32, (f64) b->data.u64));
    case TAG_UW:
      return tag_init_f64(dest, fmod((f64) a->data.f32, (f64) b->data.uw));
    default:
      goto ko;
    }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod(a->data.f64, b->data.f64));
    case TAG_INTEGER:
      return tag_init_f64(dest, fmod(a->data.f64,
                                     integer_to_f64(&b->data.integer)));
    case TAG_S8:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s8));
    case TAG_S16:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s16));
    case TAG_S32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s32));
    case TAG_S64:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s64));
    case TAG_SW:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.sw));
    case TAG_U8:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u8));
    case TAG_U16:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u16));
    case TAG_U32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u32));
    case TAG_U64:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u64));
    case TAG_UW:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.uw));
    default:
      goto ko;
    }
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod(integer_to_f64(&a->data.integer),
                                     (f64) b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod(integer_to_f64(&a->data.integer),
                                     b->data.f64));
    case TAG_INTEGER:
      dest->type = TAG_INTEGER;
      integer_mod(&a->data.integer, &b->data.integer,
                  &dest->data.integer);
      return dest;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u8(dest, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u16(dest, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u32(dest, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u64(dest, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_uw(dest, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
     case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u8(dest, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u16(dest, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u32(dest, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u64(dest, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_uw(dest, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.s8, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s8, b->data.f64));
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.s8 % b->data.s8);
    case TAG_S16:
      return tag_init_u8(dest, a->data.s8 % b->data.s16);
    case TAG_S32:
      return tag_init_u8(dest, a->data.s8 % b->data.s32);
    case TAG_S64:
      return tag_init_u8(dest, a->data.s8 % b->data.s64);
    case TAG_SW:
      return tag_init_u8(dest, a->data.s8 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.s8 % b->data.u8);
    case TAG_U16:
      return tag_init_u8(dest, a->data.s8 % b->data.u16);
    case TAG_U32:
      return tag_init_u8(dest, a->data.s8 % b->data.u32);
    case TAG_U64:
      return tag_init_u8(dest, a->data.s8 % b->data.u64);
    case TAG_UW:
      return tag_init_u8(dest, a->data.s8 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.s16, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s16, b->data.f64));
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.s16 % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.s16 % b->data.s16);
    case TAG_S32:
      return tag_init_u16(dest, a->data.s16 % b->data.s32);
    case TAG_S64:
      return tag_init_u16(dest, a->data.s16 % b->data.s64);
    case TAG_SW:
      return tag_init_u16(dest, a->data.s16 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.s16 % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.s16 % b->data.u16);
    case TAG_U32:
      return tag_init_u16(dest, a->data.s16 % b->data.u32);
    case TAG_U64:
      return tag_init_u16(dest, a->data.s16 % b->data.u64);
    case TAG_UW:
      return tag_init_u16(dest, a->data.s16 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.s32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s32, b->data.f64));
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.s32 % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.s32 % b->data.s16);
    case TAG_S32:
      return tag_init_u32(dest, a->data.s32 % b->data.s32);
    case TAG_S64:
      return tag_init_u32(dest, a->data.s32 % b->data.s64);
    case TAG_SW:
      return tag_init_u32(dest, a->data.s32 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.s32 % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.s32 % b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.s32 % b->data.u32);
    case TAG_U64:
      return tag_init_u32(dest, a->data.s32 % b->data.u64);
    case TAG_UW:
      return tag_init_u32(dest, a->data.s32 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.s64, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s64, b->data.f64));
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.s64 % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.s64 % b->data.s16);
    case TAG_S32:
      return tag_init_u32(dest, a->data.s64 % b->data.s32);
    case TAG_S64:
      return tag_init_u64(dest, a->data.s64 % b->data.s64);
    case TAG_SW:
      return tag_init_uw(dest, a->data.s64 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.s64 % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.s64 % b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.s64 % b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.s64 % b->data.u64);
    case TAG_UW:
      return tag_init_uw(dest, a->data.s64 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_SW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.sw, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.sw, b->data.f64));
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.sw % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.sw % b->data.s16);
    case TAG_S32:
      return tag_init_u32(dest, a->data.sw % b->data.s32);
    case TAG_S64:
      return tag_init_uw(dest, a->data.sw % b->data.s64);
    case TAG_SW:
      return tag_init_uw(dest, a->data.sw % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.sw % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.sw % b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.sw % b->data.u32);
    case TAG_U64:
      return tag_init_uw(dest, a->data.sw % b->data.u64);
    case TAG_UW:
      return tag_init_uw(dest, a->data.sw % b->data.uw);
    default:
      goto ko;
    }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.u8, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u8, b->data.f64));
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.u8);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.u8 % b->data.s8);
    case TAG_S16:
      return tag_init_u8(dest, a->data.u8 % b->data.s16);
    case TAG_S32:
      return tag_init_u8(dest, a->data.u8 % b->data.s32);
    case TAG_S64:
      return tag_init_u8(dest, a->data.u8 % b->data.s64);
    case TAG_SW:
      return tag_init_u8(dest, a->data.u8 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u8 % b->data.u8);
    case TAG_U16:
      return tag_init_u8(dest, a->data.u8 % b->data.u16);
    case TAG_U32:
      return tag_init_u8(dest, a->data.u8 % b->data.u32);
    case TAG_U64:
      return tag_init_u8(dest, a->data.u8 % b->data.u64);
    case TAG_UW:
      return tag_init_u8(dest, a->data.u8 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.u16, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u16, b->data.f64));
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.u16);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.u16 % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.u16 % b->data.s16);
    case TAG_S32:
      return tag_init_u16(dest, a->data.u16 % b->data.s32);
    case TAG_S64:
      return tag_init_u16(dest, a->data.u16 % b->data.s64);
    case TAG_SW:
      return tag_init_u16(dest, a->data.u16 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u16 % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u16 % b->data.u16);
    case TAG_U32:
      return tag_init_u16(dest, a->data.u16 % b->data.u32);
    case TAG_U64:
      return tag_init_u16(dest, a->data.u16 % b->data.u64);
    case TAG_UW:
      return tag_init_u16(dest, a->data.u16 % b->data.uw);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.u32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u32, b->data.f64));
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.u32);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.u32 % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.u32 % b->data.s16);
    case TAG_S32:
      return tag_init_u32(dest, a->data.u32 % b->data.s32);
    case TAG_S64:
      return tag_init_u32(dest, a->data.u32 % b->data.s64);
    case TAG_SW:
      return tag_init_u32(dest, a->data.u32 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u32 % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u32 % b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u32 % b->data.u32);
    case TAG_U64:
      return tag_init_u32(dest, a->data.u32 % b->data.u64);
    case TAG_UW:
      return tag_init_u32(dest, a->data.u32 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.u64, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u64, b->data.f64));
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.u64 % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.u64 % b->data.s16);
    case TAG_S32:
      return tag_init_u32(dest, a->data.u64 % b->data.s32);
    case TAG_S64:
      return tag_init_u64(dest, a->data.u64 % b->data.s64);
    case TAG_SW:
      return tag_init_uw(dest, a->data.u64 % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u64 % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u64 % b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u64 % b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u64 % b->data.u64);
    case TAG_UW:
      return tag_init_uw(dest, a->data.u64 % b->data.uw);
    default:
      goto ko;
    }
  case TAG_UW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.uw, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.uw, b->data.f64));
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_u8(dest, a->data.uw % b->data.s8);
    case TAG_S16:
      return tag_init_u16(dest, a->data.uw % b->data.s16);
    case TAG_S32:
      return tag_init_u32(dest, a->data.uw % b->data.s32);
    case TAG_S64:
      return tag_init_uw(dest, a->data.uw % b->data.s64);
    case TAG_SW:
      return tag_init_uw(dest, a->data.uw % b->data.sw);
    case TAG_U8:
      return tag_init_u8(dest, a->data.uw % b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.uw % b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.uw % b->data.u32);
    case TAG_U64:
      return tag_init_uw(dest, a->data.uw % b->data.u64);
    case TAG_UW:
      return tag_init_uw(dest, a->data.uw % b->data.uw);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  errx(1, "cannot divide %s by %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
}

s_tag * tag_mul (const s_tag *a, const s_tag *b, s_tag *dest)
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
      return tag_init_f32(dest, a->data.f32 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.f32 * b->data.f64);
    case TAG_INTEGER:
      return tag_init_f32(dest, a->data.f32 *
                          integer_to_f32(&b->data.integer));
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 * (f32) b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 * b->data.f64);
    case TAG_INTEGER:
      return tag_init_f64(dest, a->data.f64 *
                          integer_to_f64(&b->data.integer));
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 * (f64) b->data.u64);
    default:
      goto ko;
  }
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, integer_to_f32(&a->data.integer) *
                          b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, integer_to_f64(&a->data.integer) *
                          b->data.f64);
    case TAG_INTEGER:
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &b->data.integer,
                  &dest->data.integer);
      return dest;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&a->data.integer, &tmp, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s8 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s8 * b->data.f64);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, (s16) a->data.s8 * (s16) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.s8 * (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.s8 * (s64) b->data.s32);
    case TAG_S64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s16(dest, (s16) a->data.s8 * (s16) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, (s32) a->data.s8 * (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.s8 * (s64) b->data.u32);
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s16 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s16 * b->data.f64);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s32(dest, (s32) a->data.s16 * (s32) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.s16 * (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.s16 * (s64) b->data.s32);
    case TAG_S64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s32(dest, (s32) a->data.s16 * (s32) b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, (s32) a->data.s16 * (s32) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.s16 * (s64) b->data.u32);
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s32 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s32 * b->data.f64);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s64(dest, (s64) a->data.s32 * (s64) b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, (s64) a->data.s32 * (s64) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.s32 * (s64) b->data.s32);
    case TAG_S64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_s64(dest, (s64) a->data.s32 * (s64) b->data.u8);
    case TAG_U16:
      return tag_init_s64(dest, (s64) a->data.s32 * (s64) b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, (s64) a->data.s32 * (s64) b->data.u32);
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.s64 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.s64 * b->data.f64);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u8(&tmp2, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U16:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u16(&tmp2, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U32:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u32(&tmp2, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u8 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u8 * b->data.f64);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s16(dest, (s16) a->data.u8 * (s16) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.u8 * (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s32) a->data.u8 * (s64) b->data.s32);
    case TAG_S64:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_u16(dest, (u16) a->data.u8 * (u16) b->data.u8);
    case TAG_U16:
      return tag_init_u32(dest, (u32) a->data.u8 * (u32) b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, (u64) a->data.u8 * (u64) b->data.u32);
    case TAG_U64:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u16 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u16 * b->data.f64);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s32(dest, (s32) a->data.u16 * (s32) b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, (s32) a->data.u16 * (s32) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.u16 * (s64) b->data.s32);
    case TAG_S64:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_u32(dest, (u32) a->data.u16 * (u32) b->data.u8);
    case TAG_U16:
      return tag_init_u32(dest, (u32) a->data.u16 * (u32) b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, (u64) a->data.u16 * (u64) b->data.u32);
    case TAG_U64:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u32 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u32 * b->data.f64);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      return tag_init_s64(dest, (s64) a->data.u32 * (s64) b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, (s64) a->data.u32 * (s64) b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, (s64) a->data.u32 * b->data.s32);
    case TAG_S64:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      return tag_init_u64(dest, (u64) a->data.u32 * (u64) b->data.u8);
    case TAG_U16:
      return tag_init_u64(dest, (u64) a->data.u32 * (u64) b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, (u64) a->data.u32 * (u64) b->data.u32);
    case TAG_U64:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.u64 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.u64 * b->data.f64);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return dest;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u8(&tmp2, b->data.u8);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u16(&tmp2, b->data.u16);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u32(&tmp2, b->data.u32);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return dest;
    case TAG_U64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mul(&tmp, &tmp2, &dest->data.integer);
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
  errx(1, "cannot multiply %s by %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
}

s_tag * tag_neg (const s_tag *tag, s_tag *result)
{
  s_integer tmp;
  switch (tag->type) {
  case TAG_BOOL:
    return tag_init_s8(result, -(tag->data.bool ? 1 : 0));
  case TAG_CHARACTER:
    return tag_init_s64(result, -tag->data.character);
  case TAG_INTEGER:
    tag_init_integer_zero(result);
    integer_neg(&tag->data.integer, &result->data.integer);
    return result;
  case TAG_SW:
    if (tag->data.sw == SW_MIN) {
      integer_init_sw(&tmp, tag->data.sw);
      result->type = TAG_INTEGER;
      integer_neg(&tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    }
    return tag_init_sw(result, -tag->data.sw);
  case TAG_S64:
    if (tag->data.s64 == S64_MIN) {
      integer_init_s64(&tmp, tag->data.s64);
      result->type = TAG_INTEGER;
      integer_neg(&tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    }
    return tag_init_s64(result, -tag->data.s64);
  case TAG_S32:
    return tag_init_s64(result, -tag->data.s32);
  case TAG_S16:
    return tag_init_s32(result, -tag->data.s16);
  case TAG_S8:
    return tag_init_s16(result, -tag->data.s8);
  case TAG_U8:
    return tag_init_s16(result, -tag->data.u8);
  case TAG_U16:
    return tag_init_s32(result, -tag->data.u16);
  case TAG_U32:
    return tag_init_s64(result, -tag->data.u32);
  case TAG_U64:
    integer_init_u64(&tmp, tag->data.u64);
    result->type = TAG_INTEGER;
    integer_neg(&tmp, &result->data.integer);
    return result;
  case TAG_UW:
    integer_init_uw(&tmp, tag->data.uw);
    result->type = TAG_INTEGER;
    integer_neg(&tmp, &result->data.integer);
    return result;
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    warnx("tag_neg: invalid tag type: %s",
          tag_type_to_string(tag->type));
  }
  return NULL;
}

s_tag * tag_new ()
{
  s_tag *tag;
  tag = calloc(1, sizeof(s_tag));
  return tag;
}

s_tag * tag_new_1 (const s8 *p)
{
  s_tag *tag;
  tag = calloc(1, sizeof(s_tag));
  return tag_init_1(tag, p);
}

s_tag * tag_new_array (const s_array *a)
{
  s_tag *dest;
  assert(a);
  if (! (dest = malloc(sizeof(s_tag))))
    errx(1, "tag_new_array: out of memory");
  return tag_init_array(dest, a);
}

s_tag * tag_new_copy (const s_tag *src)
{
  s_tag *dest;
  if (! (dest = malloc(sizeof(s_tag))))
    errx(1, "tag_new_copy: out of memory");
  return tag_copy(src, dest);
}

bool tag_not (const s_tag *a)
{
  s_tag f;
  tag_init_bool(&f, false);
  return compare_tag(a, &f) == 0;
}

bool tag_not_eq (const s_tag *a, const s_tag *b)
{
  return compare_tag(a, b) != 0;
}

bool tag_or (const s_tag *a, const s_tag *b)
{
  s_tag f;
  tag_init_bool(&f, false);
  return compare_tag(a, &f) != 0 || compare_tag(b, &f) != 0;
}

s_tag * tag_paren (const s_tag *tag, s_tag *dest)
{
  assert(tag);
  assert(dest);
  return tag_copy(tag, dest);
}

s_tag * tag_s8 (s_tag *tag, s8 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s8(tag, x);
}

s_tag * tag_s16 (s_tag *tag, s16 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s16(tag, x);
}

s_tag * tag_s32 (s_tag *tag, s32 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s32(tag, x);
}

s_tag * tag_s64 (s_tag *tag, s64 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s64(tag, x);
}

s_tag * tag_shift_left (const s_tag *a, const s_tag *b, s_tag *result)
{
  s_integer tmp;
  s_integer tmp2;
  s_tag tmp_a;
  switch (a->type) {
  case TAG_BOOL:
    tmp_a.data.bool = a->data.bool ? 1 : 0;
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_bool(result, tmp_a.data.bool <<
                           (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_bool(result, tmp_a.data.bool << b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, tmp_a.data.bool);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_bool(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_bool(result, tmp_a.data.bool << b->data.sw);
    case TAG_S64:
      return tag_init_bool(result, tmp_a.data.bool << b->data.s64);
    case TAG_S32:
      return tag_init_bool(result, tmp_a.data.bool << b->data.s32);
    case TAG_S16:
      return tag_init_bool(result, tmp_a.data.bool << b->data.s16);
    case TAG_S8:
      return tag_init_bool(result, tmp_a.data.bool << b->data.s8);
    case TAG_U8:
      return tag_init_bool(result, tmp_a.data.bool << b->data.u8);
    case TAG_U16:
      return tag_init_bool(result, tmp_a.data.bool << b->data.u16);
    case TAG_U32:
      return tag_init_bool(result, tmp_a.data.bool << b->data.u32);
    case TAG_U64:
      return tag_init_bool(result, tmp_a.data.bool << b->data.u64);
    case TAG_UW:
      return tag_init_bool(result, tmp_a.data.bool << b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_CHARACTER:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_character(result, a->data.character <<
                                (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_character(result, a->data.character << b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.character);
      integer_lshift(&tmp, integer_to_sw(&b->data.integer), &tmp2);
      tag_init_character(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_character(result, a->data.character << b->data.sw);
    case TAG_S64:
      return tag_init_character(result, a->data.character << b->data.s64);
    case TAG_S32:
      return tag_init_character(result, a->data.character << b->data.s32);
    case TAG_S16:
      return tag_init_character(result, a->data.character << b->data.s16);
    case TAG_S8:
      return tag_init_character(result, a->data.character << b->data.s8);
    case TAG_U8:
      return tag_init_character(result, a->data.character << b->data.u8);
    case TAG_U16:
      return tag_init_character(result, a->data.character << b->data.u16);
    case TAG_U32:
      return tag_init_character(result, a->data.character << b->data.u32);
    case TAG_U64:
      return tag_init_character(result, a->data.character << b->data.u64);
    case TAG_UW:
      return tag_init_character(result, a->data.character << b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_BOOL:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.bool ? 1 : 0,
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
      integer_lshift(&a->data.integer, b->data.s64, &result->data.integer);
      return result;
    case TAG_S32:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s32, &result->data.integer);
      return result;
    case TAG_S16:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s16, &result->data.integer);
      return result;
    case TAG_S8:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.s8, &result->data.integer);
      return result;
    case TAG_U8:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u8, &result->data.integer);
      return result;
    case TAG_U16:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u16, &result->data.integer);
      return result;
    case TAG_U32:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u32, &result->data.integer);
      return result;
    case TAG_U64:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.u64, &result->data.integer);
      return result;
    case TAG_UW:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.uw, &result->data.integer);
      return result;
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_sw(result, a->data.sw <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s64(result, a->data.s64 <<
                          (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s32(result, a->data.s32 <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s16(result, a->data.s16 <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s8(result, a->data.s8 <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 <<
                          (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw <<
                         (b->data.bool ? 1 : 0));
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
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto error;
  }
 error:
  warnx("tag_shift_left: invalid tag type: %s << %s",
        tag_type_to_string(a->type),
        tag_type_to_string(b->type));
  return NULL;
}

s_tag * tag_shift_right (const s_tag *a, const s_tag *b, s_tag *result)
{
  s_integer tmp;
  s_integer tmp2;
  s_tag tmp_a;
  switch (a->type) {
  case TAG_BOOL:
    tmp_a.data.bool = a->data.bool ? 1 : 0;
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_bool(result, tmp_a.data.bool >>
                           (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, tmp_a.data.bool);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_bool(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.sw);
    case TAG_S64:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.s64);
    case TAG_S32:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.s32);
    case TAG_S16:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.s16);
    case TAG_S8:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.s8);
    case TAG_U8:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.u8);
    case TAG_U16:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.u16);
    case TAG_U32:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.u32);
    case TAG_U64:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.u64);
    case TAG_UW:
      return tag_init_bool(result, tmp_a.data.bool >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_CHARACTER:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_character(result, a->data.character >>
                                (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_character(result, a->data.character >> b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.character);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_character(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_character(result, a->data.character >> b->data.sw);
    case TAG_S64:
      return tag_init_character(result, a->data.character >> b->data.s64);
    case TAG_S32:
      return tag_init_character(result, a->data.character >> b->data.s32);
    case TAG_S16:
      return tag_init_character(result, a->data.character >> b->data.s16);
    case TAG_S8:
      return tag_init_character(result, a->data.character >> b->data.s8);
    case TAG_U8:
      return tag_init_character(result, a->data.character >> b->data.u8);
    case TAG_U16:
      return tag_init_character(result, a->data.character >> b->data.u16);
    case TAG_U32:
      return tag_init_character(result, a->data.character >> b->data.u32);
    case TAG_U64:
      return tag_init_character(result, a->data.character >> b->data.u64);
    case TAG_UW:
      return tag_init_character(result, a->data.character >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_BOOL:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, b->data.bool ? -1 : 0,
                     &result->data.integer);
      return result;
    case TAG_CHARACTER:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.character,
                     &result->data.integer);
      return result;
    case TAG_INTEGER:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -integer_to_sw(&b->data.integer),
                     &result->data.integer);
      return result;
    case TAG_SW:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.sw,
                     &result->data.integer);
      return result;
    case TAG_S64:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.s64, &result->data.integer);
      return result;
    case TAG_S32:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.s32, &result->data.integer);
      return result;
    case TAG_S16:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.s16, &result->data.integer);
      return result;
    case TAG_S8:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.s8, &result->data.integer);
      return result;
    case TAG_U8:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.u8, &result->data.integer);
      return result;
    case TAG_U16:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.u16, &result->data.integer);
      return result;
    case TAG_U32:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.u32, &result->data.integer);
      return result;
    case TAG_U64:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.u64, &result->data.integer);
      return result;
    case TAG_UW:
      result->type = TAG_INTEGER;
      integer_lshift(&a->data.integer, -b->data.uw, &result->data.integer);
      return result;
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_SW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_sw(result, a->data.sw >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_sw(result, a->data.sw >> b->data.character);
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_sw(result, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_sw(result, a->data.sw >> b->data.sw);
    case TAG_S64:
      return tag_init_sw(result, a->data.sw >> b->data.s64);
    case TAG_S32:
      return tag_init_sw(result, a->data.sw >> b->data.s32);
    case TAG_S16:
      return tag_init_sw(result, a->data.sw >> b->data.s16);
    case TAG_S8:
      return tag_init_sw(result, a->data.sw >> b->data.s8);
    case TAG_U8:
      return tag_init_sw(result, a->data.sw >> b->data.u8);
    case TAG_U16:
      return tag_init_sw(result, a->data.sw >> b->data.u16);
    case TAG_U32:
      return tag_init_sw(result, a->data.sw >> b->data.u32);
    case TAG_U64:
      return tag_init_sw(result, a->data.sw >> b->data.u64);
    case TAG_UW:
      return tag_init_sw(result, a->data.sw >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s64(result, a->data.s64 >>
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s64(result, a->data.s64 >> b->data.character);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s64(result, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s64(result, a->data.s64 >> b->data.sw);
    case TAG_S64:
      return tag_init_s64(result, a->data.s64 >> b->data.s64);
    case TAG_S32:
      return tag_init_s64(result, a->data.s64 >> b->data.s32);
    case TAG_S16:
      return tag_init_s64(result, a->data.s64 >> b->data.s16);
    case TAG_S8:
      return tag_init_s64(result, a->data.s64 >> b->data.s8);
    case TAG_U8:
      return tag_init_s64(result, a->data.s64 >> b->data.u8);
    case TAG_U16:
      return tag_init_s64(result, a->data.s64 >> b->data.u16);
    case TAG_U32:
      return tag_init_s64(result, a->data.s64 >> b->data.u32);
    case TAG_U64:
      return tag_init_s64(result, a->data.s64 >> b->data.u64);
    case TAG_UW:
      return tag_init_s64(result, a->data.s64 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s32(result, a->data.s32 >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s32(result, a->data.s32 >> b->data.character);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s32(result, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s32(result, a->data.s32 >> b->data.sw);
    case TAG_S64:
      return tag_init_s32(result, a->data.s32 >> b->data.s64);
    case TAG_S32:
      return tag_init_s32(result, a->data.s32 >> b->data.s32);
    case TAG_S16:
      return tag_init_s32(result, a->data.s32 >> b->data.s16);
    case TAG_S8:
      return tag_init_s32(result, a->data.s32 >> b->data.s8);
    case TAG_U8:
      return tag_init_s32(result, a->data.s32 >> b->data.u8);
    case TAG_U16:
      return tag_init_s32(result, a->data.s32 >> b->data.u16);
    case TAG_U32:
      return tag_init_s32(result, a->data.s32 >> b->data.u32);
    case TAG_U64:
      return tag_init_s32(result, a->data.s32 >> b->data.u64);
    case TAG_UW:
      return tag_init_s32(result, a->data.s32 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s16(result, a->data.s16 >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s16(result, a->data.s16 >> b->data.character);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s16(result, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s16(result, a->data.s16 >> b->data.sw);
    case TAG_S64:
      return tag_init_s16(result, a->data.s16 >> b->data.s64);
    case TAG_S32:
      return tag_init_s16(result, a->data.s16 >> b->data.s32);
    case TAG_S16:
      return tag_init_s16(result, a->data.s16 >> b->data.s16);
    case TAG_S8:
      return tag_init_s16(result, a->data.s16 >> b->data.s8);
    case TAG_U8:
      return tag_init_s16(result, a->data.s16 >> b->data.u8);
    case TAG_U16:
      return tag_init_s16(result, a->data.s16 >> b->data.u16);
    case TAG_U32:
      return tag_init_s16(result, a->data.s16 >> b->data.u32);
    case TAG_U64:
      return tag_init_s16(result, a->data.s16 >> b->data.u64);
    case TAG_UW:
      return tag_init_s16(result, a->data.s16 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_S8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_s8(result, a->data.s8 >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_s8(result, a->data.s8 >> b->data.character);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_s8(result, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_s8(result, a->data.s8 >> b->data.sw);
    case TAG_S64:
      return tag_init_s8(result, a->data.s8 >> b->data.s64);
    case TAG_S32:
      return tag_init_s8(result, a->data.s8 >> b->data.s32);
    case TAG_S16:
      return tag_init_s8(result, a->data.s8 >> b->data.s16);
    case TAG_S8:
      return tag_init_s8(result, a->data.s8 >> b->data.s8);
    case TAG_U8:
      return tag_init_s8(result, a->data.s8 >> b->data.u8);
    case TAG_U16:
      return tag_init_s8(result, a->data.s8 >> b->data.u16);
    case TAG_U32:
      return tag_init_s8(result, a->data.s8 >> b->data.u32);
    case TAG_U64:
      return tag_init_s8(result, a->data.s8 >> b->data.u64);
    case TAG_UW:
      return tag_init_s8(result, a->data.s8 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U8:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u8(result, a->data.u8 >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u8(result, a->data.u8 >> b->data.character);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u8(result, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u8(result, a->data.u8 >> b->data.sw);
    case TAG_S64:
      return tag_init_u8(result, a->data.u8 >> b->data.s64);
    case TAG_S32:
      return tag_init_u8(result, a->data.u8 >> b->data.s32);
    case TAG_S16:
      return tag_init_u8(result, a->data.u8 >> b->data.s16);
    case TAG_S8:
      return tag_init_u8(result, a->data.u8 >> b->data.s8);
    case TAG_U8:
      return tag_init_u8(result, a->data.u8 >> b->data.u8);
    case TAG_U16:
      return tag_init_u8(result, a->data.u8 >> b->data.u16);
    case TAG_U32:
      return tag_init_u8(result, a->data.u8 >> b->data.u32);
    case TAG_U64:
      return tag_init_u8(result, a->data.u8 >> b->data.u64);
    case TAG_UW:
      return tag_init_u8(result, a->data.u8 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U16:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u16(result, a->data.u16 >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u16(result, a->data.u16 >> b->data.character);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u16(result, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u16(result, a->data.u16 >> b->data.sw);
    case TAG_S64:
      return tag_init_u16(result, a->data.u16 >> b->data.s64);
    case TAG_S32:
      return tag_init_u16(result, a->data.u16 >> b->data.s32);
    case TAG_S16:
      return tag_init_u16(result, a->data.u16 >> b->data.s16);
    case TAG_S8:
      return tag_init_u16(result, a->data.u16 >> b->data.s8);
    case TAG_U8:
      return tag_init_u16(result, a->data.u16 >> b->data.u8);
    case TAG_U16:
      return tag_init_u16(result, a->data.u16 >> b->data.u16);
    case TAG_U32:
      return tag_init_u16(result, a->data.u16 >> b->data.u32);
    case TAG_U64:
      return tag_init_u16(result, a->data.u16 >> b->data.u64);
    case TAG_UW:
      return tag_init_u16(result, a->data.u16 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U32:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u32(result, a->data.u32 >>
                          (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u32(result, a->data.u32 >> b->data.character);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u32(result, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u32(result, a->data.u32 >> b->data.sw);
    case TAG_S64:
      return tag_init_u32(result, a->data.u32 >> b->data.s64);
    case TAG_S32:
      return tag_init_u32(result, a->data.u32 >> b->data.s32);
    case TAG_S16:
      return tag_init_u32(result, a->data.u32 >> b->data.s16);
    case TAG_S8:
      return tag_init_u32(result, a->data.u32 >> b->data.s8);
    case TAG_U8:
      return tag_init_u32(result, a->data.u32 >> b->data.u8);
    case TAG_U16:
      return tag_init_u32(result, a->data.u32 >> b->data.u16);
    case TAG_U32:
      return tag_init_u32(result, a->data.u32 >> b->data.u32);
    case TAG_U64:
      return tag_init_u32(result, a->data.u32 >> b->data.u64);
    case TAG_UW:
      return tag_init_u32(result, a->data.u32 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_U64:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_u64(result, a->data.u64 >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_u64(result, a->data.u64 >> b->data.character);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_u64(result, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_u64(result, a->data.u64 >> b->data.sw);
    case TAG_S64:
      return tag_init_u64(result, a->data.u64 >> b->data.s64);
    case TAG_S32:
      return tag_init_u64(result, a->data.u64 >> b->data.s32);
    case TAG_S16:
      return tag_init_u64(result, a->data.u64 >> b->data.s16);
    case TAG_S8:
      return tag_init_u64(result, a->data.u64 >> b->data.s8);
    case TAG_U8:
      return tag_init_u64(result, a->data.u64 >> b->data.u8);
    case TAG_U16:
      return tag_init_u64(result, a->data.u64 >> b->data.u16);
    case TAG_U32:
      return tag_init_u64(result, a->data.u64 >> b->data.u32);
    case TAG_U64:
      return tag_init_u64(result, a->data.u64 >> b->data.u64);
    case TAG_UW:
      return tag_init_u64(result, a->data.u64 >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_UW:
    switch (b->type) {
    case TAG_BOOL:
      return tag_init_uw(result, a->data.uw >>
                         (b->data.bool ? 1 : 0));
    case TAG_CHARACTER:
      return tag_init_uw(result, a->data.uw >> b->data.character);
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      integer_lshift(&tmp, -integer_to_sw(&b->data.integer), &tmp2);
      tag_init_uw(result, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return result;
    case TAG_SW:
      return tag_init_uw(result, a->data.uw >> b->data.sw);
    case TAG_S64:
      return tag_init_uw(result, a->data.uw >> b->data.s64);
    case TAG_S32:
      return tag_init_uw(result, a->data.uw >> b->data.s32);
    case TAG_S16:
      return tag_init_uw(result, a->data.uw >> b->data.s16);
    case TAG_S8:
      return tag_init_uw(result, a->data.uw >> b->data.s8);
    case TAG_U8:
      return tag_init_uw(result, a->data.uw >> b->data.u8);
    case TAG_U16:
      return tag_init_uw(result, a->data.uw >> b->data.u16);
    case TAG_U32:
      return tag_init_uw(result, a->data.uw >> b->data.u32);
    case TAG_U64:
      return tag_init_uw(result, a->data.uw >> b->data.u64);
    case TAG_UW:
      return tag_init_uw(result, a->data.uw >> b->data.uw);
    case TAG_VOID:
    case TAG_ARRAY:
    case TAG_CALL:
    case TAG_CFN:
    case TAG_F32:
    case TAG_F64:
    case TAG_FACT:
    case TAG_FN:
    case TAG_IDENT:
    case TAG_LIST:
    case TAG_PTAG:
    case TAG_QUOTE:
    case TAG_STR:
    case TAG_SYM:
    case TAG_TUPLE:
    case TAG_VAR:
      goto error;
    }
    goto error;
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto error;
  }
 error:
  warnx("tag_shift_right: invalid tag type: %s >> %s",
        tag_type_to_string(a->type),
        tag_type_to_string(b->type));
  return NULL;
}

s_tag * tag_sw (s_tag *tag, sw x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_sw(tag, x);
}

s_tag * tag_str (s_tag *tag, s8 *free, uw size, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_str(tag, free, size, p);
}

s_tag * tag_str_1 (s_tag *tag, s8 *free, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_str_1(tag, free, p);
}

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
  errx(1, "cannot subtract %s by %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
}

s_tag * tag_sym (s_tag *tag, const s_sym *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_sym(tag, x);
}

s_tag * tag_sym_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_sym_1(tag, p);
}

void * tag_to_ffi_pointer (s_tag *tag, const s_sym *type)
{
  if (type == sym_1("tag"))
    return tag;
  switch (tag->type) {
  case TAG_VOID:
    if (type == sym_1("void"))
      return NULL;
    goto invalid_type;
  case TAG_ARRAY:
    if (type == sym_1("array"))
      return tag->data.array.data;
    goto invalid_type;
  case TAG_BOOL:
    if (type == sym_1("bool"))
      return &tag->data.bool;
    goto invalid_type;
  case TAG_CALL:
    if (type == sym_1("call"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CFN:
    if (type == sym_1("cfn"))
      return &tag->data.cfn;
    goto invalid_type;
  case TAG_CHARACTER:
    if (type == sym_1("character"))
      return &tag->data.character;
    goto invalid_type;
  case TAG_F32:
    if (type == sym_1("f32"))
      return &tag->data.f32;
    goto invalid_type;
  case TAG_F64:
    if (type == sym_1("f64"))
      return &tag->data.f64;
    goto invalid_type;
  case TAG_FACT:
    if (type == sym_1("fact"))
      return &tag->data.fact;
    goto invalid_type;
  case TAG_FN:
    if (type == sym_1("fn"))
      return &tag->data.fn;
    goto invalid_type;
  case TAG_IDENT:
    if (type == sym_1("ident"))
      return &tag->data.ident;
    goto invalid_type;
  case TAG_INTEGER:
    if (type == sym_1("integer"))
      return &tag->data.integer;
    goto invalid_type;
  case TAG_SW:
    if (type == sym_1("sw"))
      return &tag->data.sw;
    goto invalid_type;
  case TAG_S64:
    if (type == sym_1("s64"))
      return &tag->data.s64;
    goto invalid_type;
  case TAG_S32:
    if (type == sym_1("s32"))
      return &tag->data.s32;
    goto invalid_type;
  case TAG_S16:
    if (type == sym_1("s16"))
      return &tag->data.s16;
    goto invalid_type;
  case TAG_S8:
    if (type == sym_1("s8"))
      return &tag->data.s8;
    goto invalid_type;
  case TAG_U8:
    if (type == sym_1("u8"))
      return &tag->data.u8;
    goto invalid_type;
  case TAG_U16:
    if (type == sym_1("u16"))
      return &tag->data.u16;
    goto invalid_type;
  case TAG_U32:
    if (type == sym_1("u32"))
      return &tag->data.u32;
    goto invalid_type;
  case TAG_U64:
    if (type == sym_1("u64"))
      return &tag->data.u64;
    goto invalid_type;
  case TAG_UW:
    if (type == sym_1("uw"))
      return &tag->data.uw;
    goto invalid_type;
  case TAG_LIST:
    if (type == sym_1("list"))
      return &tag->data.list;
    goto invalid_type;
  case TAG_PTAG:
    if (type == sym_1("ptag"))
      return (void *) tag->data.ptag;
    goto invalid_type;
  case TAG_QUOTE:
    if (type == sym_1("quote"))
      return &tag->data.quote;
    goto invalid_type;
  case TAG_STR:
    if (type == sym_1("str"))
      return &tag->data.str;
    if (type == sym_1("char*"))
      return (void *) tag->data.str.ptr.ps8;
    goto invalid_type;
  case TAG_SYM:
    if (type == sym_1("sym"))
      return (void *) tag->data.sym;
    if (type == sym_1("str"))
      return (void *) &tag->data.sym->str;
    if (type == sym_1("char*"))
      return (void *) tag->data.sym->str.ptr.ps8;
    goto invalid_type;
  case TAG_TUPLE:
    if (type == sym_1("tuple"))
      return &tag->data.tuple;
    goto invalid_type;
  case TAG_VAR:
    goto invalid_type;
  }
  assert(! "tag_to_ffi_pointer: invalid tag type");
  errx(1, "tag_to_ffi_pointer: invalid tag type");
  return NULL;
 invalid_type:
  warnx("tag_to_ffi_pointer: cannot cast %s to %s",
        tag_type_to_sym(tag->type)->str.ptr.ps8,
        type->str.ptr.ps8);
  return NULL;
}

void * tag_to_pointer (s_tag *tag, e_tag_type type)
{
  if (tag->type != type) {
    warnx("tag_to_pointer: cannot cast %s to %s",
          tag_type_to_sym(tag->type)->str.ptr.ps8,
          tag_type_to_sym(type)->str.ptr.ps8);
    return NULL;
  }
  switch (tag->type) {
  case TAG_VOID:
    return NULL;
  case TAG_ARRAY:
    return tag->data.array.data;
  case TAG_BOOL:
    return &tag->data.bool;
  case TAG_CALL:
    return &tag->data.call;
  case TAG_CFN:
    return &tag->data.cfn;
  case TAG_CHARACTER:
    return &tag->data.character;
  case TAG_F32:
    return &tag->data.f32;
  case TAG_F64:
    return &tag->data.f64;
  case TAG_FACT:
    return &tag->data.fact;
  case TAG_FN:
    return &tag->data.fn;
  case TAG_IDENT:
    return &tag->data.ident;
  case TAG_INTEGER:
    return &tag->data.integer;
  case TAG_SW:
    return &tag->data.sw;
  case TAG_S64:
    return &tag->data.s64;
  case TAG_S32:
    return &tag->data.s32;
  case TAG_S16:
    return &tag->data.s16;
  case TAG_S8:
    return &tag->data.s8;
  case TAG_U8:
    return &tag->data.u8;
  case TAG_U16:
    return &tag->data.u16;
  case TAG_U32:
    return &tag->data.u32;
  case TAG_U64:
    return &tag->data.u64;
  case TAG_UW:
    return &tag->data.uw;
  case TAG_LIST:
    return &tag->data.list;
  case TAG_PTAG:
    return &tag->data.ptag;
  case TAG_QUOTE:
    return &tag->data.quote;
  case TAG_STR:
    return &tag->data.str;
  case TAG_SYM:
    return (void *) tag->data.sym;
  case TAG_TUPLE:
    return &tag->data.tuple;
  case TAG_VAR:
    goto invalid_type;
  }
  assert(! "tag_to_pointer: invalid tag type");
  errx(1, "tag_to_pointer: invalid tag type");
  return NULL;
 invalid_type:
  warnx("tag_to_pointer: cannot cast %s to %s",
        tag_type_to_sym(tag->type)->str.ptr.ps8,
        tag_type_to_sym(type)->str.ptr.ps8);
  return NULL;

}

sw tag_type_size (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return 0;
  case TAG_ARRAY:
    assert(! "tag_type_size: TAG_ARRAY: not implemented");
    errx(1, "tag_type_size: TAG_ARRAY: not implemented");
    return -1;
  case TAG_BOOL:
    return sizeof(bool);
  case TAG_CALL:
    return sizeof(s_call);
  case TAG_CFN:
    return sizeof(s_cfn);
  case TAG_CHARACTER:
    return sizeof(character);
  case TAG_F32:
    return sizeof(f32);
  case TAG_F64:
    return sizeof(f64);
  case TAG_FACT:
    return sizeof(s_fact);
  case TAG_FN:
    return sizeof(s_fn);
  case TAG_IDENT:
    return sizeof(s_ident);
  case TAG_INTEGER:
    return sizeof(s_integer);
  case TAG_SW:
    return sizeof(sw);
  case TAG_S64:
    return sizeof(s64);
  case TAG_S32:
    return sizeof(s32);
  case TAG_S16:
    return sizeof(s16);
  case TAG_S8:
    return sizeof(s8);
  case TAG_U8:
    return sizeof(u8);
  case TAG_U16:
    return sizeof(u16);
  case TAG_U32:
    return sizeof(u32);
  case TAG_U64:
    return sizeof(u64);
  case TAG_UW:
    return sizeof(uw);
  case TAG_LIST:
    return sizeof(s_list *);
  case TAG_PTAG:
    return sizeof(p_tag);
  case TAG_QUOTE:
    return sizeof(s_quote);
  case TAG_STR:
    return sizeof(s_str);
  case TAG_SYM:
    return sizeof(s_sym *);
  case TAG_TUPLE:
    return sizeof(s_tuple);
  case TAG_VAR:
    return sizeof(s_tag);
  }
  assert(! "tag_type_size: invalid tag type");
  errx(1, "tag_type_size: invalid tag type: %d", type);
  return -1;
}

f_buf_inspect tag_type_to_buf_inspect (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_inspect) buf_inspect_void;
  case TAG_ARRAY:
  case TAG_BOOL:
    return (f_buf_inspect) buf_inspect_bool;
  case TAG_CALL:
    return (f_buf_inspect) buf_inspect_call;
  case TAG_CFN:
    return (f_buf_inspect) buf_inspect_cfn;
  case TAG_CHARACTER:
    return (f_buf_inspect) buf_inspect_character;
  case TAG_F32:
    return (f_buf_inspect) buf_inspect_f32;
  case TAG_F64:
    return (f_buf_inspect) buf_inspect_f64;
  case TAG_FACT:
    return (f_buf_inspect) buf_inspect_fact;
  case TAG_FN:
    return (f_buf_inspect) buf_inspect_fn;
  case TAG_IDENT:
    return (f_buf_inspect) buf_inspect_ident;
  case TAG_INTEGER:
    return (f_buf_inspect) buf_inspect_integer;
  case TAG_SW:
    return (f_buf_inspect) buf_inspect_sw;
  case TAG_S64:
    return (f_buf_inspect) buf_inspect_s64;
  case TAG_S32:
    return (f_buf_inspect) buf_inspect_s32;
  case TAG_S16:
    return (f_buf_inspect) buf_inspect_s16;
  case TAG_S8:
    return (f_buf_inspect) buf_inspect_s8;
  case TAG_U8:
    return (f_buf_inspect) buf_inspect_u8;
  case TAG_U16:
    return (f_buf_inspect) buf_inspect_u16;
  case TAG_U32:
    return (f_buf_inspect) buf_inspect_u32;
  case TAG_U64:
    return (f_buf_inspect) buf_inspect_u64;
  case TAG_UW:
    return (f_buf_inspect) buf_inspect_uw;
  case TAG_LIST:
    return (f_buf_inspect) buf_inspect_list;
  case TAG_PTAG:
    return (f_buf_inspect) buf_inspect_ptag;
  case TAG_QUOTE:
    return (f_buf_inspect) buf_inspect_quote;
  case TAG_STR:
    return (f_buf_inspect) buf_inspect_str;
  case TAG_SYM:
    return (f_buf_inspect) buf_inspect_sym;
  case TAG_TUPLE:
    return (f_buf_inspect) buf_inspect_tuple;
  case TAG_VAR:
    return (f_buf_inspect) buf_inspect_var;
  }
  assert(! "buf_inspect: unknown tag type");
  errx(1, "buf_inspect: unknown tag type");
  return NULL;
}

f_buf_inspect_size tag_type_to_buf_inspect_size (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_inspect_size) buf_inspect_void_size;
  case TAG_ARRAY:
  case TAG_BOOL:
    return (f_buf_inspect_size) buf_inspect_bool_size;
  case TAG_CALL:
    return (f_buf_inspect_size) buf_inspect_call_size;
  case TAG_CFN:
    return (f_buf_inspect_size) buf_inspect_cfn_size;
  case TAG_CHARACTER:
    return (f_buf_inspect_size) buf_inspect_character_size;
  case TAG_F32:
    return (f_buf_inspect_size) buf_inspect_f32_size;
  case TAG_F64:
    return (f_buf_inspect_size) buf_inspect_f64_size;
  case TAG_FACT:
    return (f_buf_inspect_size) buf_inspect_fact_size;
  case TAG_FN:
    return (f_buf_inspect_size) buf_inspect_fn_size;
  case TAG_IDENT:
    return (f_buf_inspect_size) buf_inspect_ident_size;
  case TAG_INTEGER:
    return (f_buf_inspect_size) buf_inspect_integer_size;
  case TAG_SW:
    return (f_buf_inspect_size) buf_inspect_sw_size;
  case TAG_S64:
    return (f_buf_inspect_size) buf_inspect_s64_size;
  case TAG_S32:
    return (f_buf_inspect_size) buf_inspect_s32_size;
  case TAG_S16:
    return (f_buf_inspect_size) buf_inspect_s16_size;
  case TAG_S8:
    return (f_buf_inspect_size) buf_inspect_s8_size;
  case TAG_U8:
    return (f_buf_inspect_size) buf_inspect_u8_size;
  case TAG_U16:
    return (f_buf_inspect_size) buf_inspect_u16_size;
  case TAG_U32:
    return (f_buf_inspect_size) buf_inspect_u32_size;
  case TAG_U64:
    return (f_buf_inspect_size) buf_inspect_u64_size;
  case TAG_UW:
    return (f_buf_inspect_size) buf_inspect_uw_size;
  case TAG_LIST:
    return (f_buf_inspect_size) buf_inspect_list_size;
  case TAG_PTAG:
    return (f_buf_inspect_size) buf_inspect_ptag_size;
  case TAG_QUOTE:
    return (f_buf_inspect_size) buf_inspect_quote_size;
  case TAG_STR:
    return (f_buf_inspect_size) buf_inspect_str_size;
  case TAG_SYM:
    return (f_buf_inspect_size) buf_inspect_sym_size;
  case TAG_TUPLE:
    return (f_buf_inspect_size) buf_inspect_tuple_size;
  case TAG_VAR:
    return (f_buf_inspect_size) buf_inspect_var_size;
  }
  assert(! "tag_type_to_buf_inspect_size: unknown tag type");
  errx(1, "tag_type_to_buf_inspect_size: unknown tag type");
  return NULL;
}

f_buf_parse tag_type_to_buf_parse (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_parse) buf_parse_void;
  case TAG_ARRAY:
    return (f_buf_parse) buf_parse_array;
  case TAG_BOOL:
    return (f_buf_parse) buf_parse_bool;
  case TAG_CALL:
    return (f_buf_parse) buf_parse_call;
  case TAG_CFN:
    return (f_buf_parse) buf_parse_cfn;
  case TAG_CHARACTER:
    return (f_buf_parse) buf_parse_character;
  case TAG_F32:
    return (f_buf_parse) buf_parse_f32;
  case TAG_F64:
    return (f_buf_parse) buf_parse_f64;
  case TAG_FACT:
    return (f_buf_parse) buf_parse_fact;
  case TAG_FN:
    return (f_buf_parse) buf_parse_fn;
  case TAG_IDENT:
    return (f_buf_parse) buf_parse_ident;
  case TAG_INTEGER:
    return (f_buf_parse) buf_parse_integer;
  case TAG_SW:
    return (f_buf_parse) buf_parse_sw;
  case TAG_S64:
    return (f_buf_parse) buf_parse_s64;
  case TAG_S32:
    return (f_buf_parse) buf_parse_s32;
  case TAG_S16:
    return (f_buf_parse) buf_parse_s16;
  case TAG_S8:
    return (f_buf_parse) buf_parse_s8;
  case TAG_U8:
    return (f_buf_parse) buf_parse_u8;
  case TAG_U16:
    return (f_buf_parse) buf_parse_u16;
  case TAG_U32:
    return (f_buf_parse) buf_parse_u32;
  case TAG_U64:
    return (f_buf_parse) buf_parse_u64;
  case TAG_UW:
    return (f_buf_parse) buf_parse_uw;
  case TAG_LIST:
    return (f_buf_parse) buf_parse_list;
  case TAG_PTAG:
    return (f_buf_parse) buf_parse_ptag;
  case TAG_QUOTE:
    return (f_buf_parse) buf_parse_quote;
  case TAG_STR:
    return (f_buf_parse) buf_parse_str;
  case TAG_SYM:
    return (f_buf_parse) buf_parse_sym;
  case TAG_TUPLE:
    return (f_buf_parse) buf_parse_tuple;
  case TAG_VAR:
    return (f_buf_parse) buf_parse_var;
  }
  assert(! "tag_type_to_buf_parse: invalid tag type");
  err(1, "tag_type_to_buf_parse: invalid tag type");
  return NULL;
}

ffi_type * tag_type_to_ffi_type (e_tag_type type)
{
  switch (type) {
  case TAG_ARRAY:
    return &ffi_type_pointer;
  case TAG_BOOL:
    return &ffi_type_uint8;
  case TAG_CALL:
    return &ffi_type_pointer;
  case TAG_CFN:
    return &ffi_type_pointer;
  case TAG_CHARACTER:
    return &ffi_type_schar;
  case TAG_F32:
    return &ffi_type_float;
  case TAG_F64:
    return &ffi_type_double;
  case TAG_FACT:
    return &ffi_type_pointer;
  case TAG_FN:
    return &ffi_type_pointer;
  case TAG_IDENT:
    return &ffi_type_pointer;
  case TAG_INTEGER:
    return &ffi_type_sint;
  case TAG_LIST:
    return &ffi_type_pointer;
  case TAG_PTAG:
    return &ffi_type_pointer;
  case TAG_QUOTE:
    return &ffi_type_pointer;
  case TAG_S8:
    return &ffi_type_sint8;
  case TAG_S16:
    return &ffi_type_sint16;
  case TAG_S32:
    return &ffi_type_sint32;
  case TAG_S64:
    return &ffi_type_sint64;
  case TAG_SW:
    return &ffi_type_slong;
  case TAG_STR:
    return &ffi_type_pointer;
  case TAG_SYM:
    return &ffi_type_pointer;
  case TAG_TUPLE:
    return &ffi_type_pointer;
  case TAG_U8:
    return &ffi_type_uint8;
  case TAG_U16:
    return &ffi_type_uint16;
  case TAG_U32:
    return &ffi_type_uint32;
  case TAG_U64:
    return &ffi_type_uint64;
  case TAG_UW:
    return &ffi_type_ulong;
  case TAG_VAR:
    return &ffi_type_pointer;
  case TAG_VOID:
    return &ffi_type_void;
  }
  assert(! "tag_type_to_ffi_type: unknown tag type");
  errx(1, "tag_type_to_ffi_type: unknown tag type");
  return &ffi_type_void;
}

const s_sym * tag_type_to_module (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:       return sym_1("Void");
  case TAG_ARRAY:      return sym_1("Array");
  case TAG_BOOL:       return sym_1("Bool");
  case TAG_CALL:       return sym_1("Call");
  case TAG_CFN:        return sym_1("Cfn");
  case TAG_CHARACTER:  return sym_1("Character");
  case TAG_F32:        return sym_1("F32");
  case TAG_F64:        return sym_1("F64");
  case TAG_FACT:       return sym_1("Fact");
  case TAG_FN:         return sym_1("Fn");
  case TAG_IDENT:      return sym_1("Ident");
  case TAG_INTEGER:    return sym_1("Integer");
  case TAG_SW:         return sym_1("Sw");
  case TAG_S64:        return sym_1("S64");
  case TAG_S32:        return sym_1("S32");
  case TAG_S16:        return sym_1("S16");
  case TAG_S8:         return sym_1("S8");
  case TAG_U8:         return sym_1("U8");
  case TAG_U16:        return sym_1("U16");
  case TAG_U32:        return sym_1("U32");
  case TAG_U64:        return sym_1("U64");
  case TAG_UW:         return sym_1("Uw");
  case TAG_LIST:       return sym_1("List");
  case TAG_PTAG:       return sym_1("Ptag");
  case TAG_QUOTE:      return sym_1("Quote");
  case TAG_STR:        return sym_1("Str");
  case TAG_SYM:        return sym_1("Sym");
  case TAG_TUPLE:      return sym_1("Tuple");
  case TAG_VAR:        return sym_1("Var");
  }
  assert(! "tag_type_to_module: invalid tag type");
  errx(1, "tag_type_to_module: invalid tag type: %d", tag_type);
  return NULL;
}

s8 * tag_type_to_string (e_tag_type type)
{
  switch (type) {
  case TAG_VOID: return "void";
  case TAG_ARRAY: return "array";
  case TAG_BOOL: return "bool";
  case TAG_CALL: return "call";
  case TAG_CFN: return "cfn";
  case TAG_CHARACTER: return "character";
  case TAG_F32: return "f32";
  case TAG_F64: return "f64";
  case TAG_FACT: return "fact";
  case TAG_FN: return "fn";
  case TAG_IDENT: return "ident";
  case TAG_INTEGER: return "integer";
  case TAG_SW: return "sw";
  case TAG_S64: return "s64";
  case TAG_S32: return "s32";
  case TAG_S16: return "s16";
  case TAG_S8: return "s8";
  case TAG_U8: return "u8";
  case TAG_U16: return "u16";
  case TAG_U32: return "u32";
  case TAG_U64: return "u64";
  case TAG_UW: return "uw";
  case TAG_LIST: return "list";
  case TAG_PTAG: return "ptag";
  case TAG_QUOTE: return "quote";
  case TAG_STR: return "str";
  case TAG_SYM: return "sym";
  case TAG_TUPLE: return "tuple";
  case TAG_VAR: return "var";
  }
  assert(! "tag_type_to_string: unknown tag type");
  errx(1, "tag_type_to_string: unknown tag type");
  return NULL;
}

const s_sym * tag_type_to_sym (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:       return sym_1("void");
  case TAG_ARRAY:      return sym_1("array");
  case TAG_BOOL:       return sym_1("bool");
  case TAG_CALL:       return sym_1("call");
  case TAG_CFN:        return sym_1("cfn");
  case TAG_CHARACTER:  return sym_1("character");
  case TAG_F32:        return sym_1("f32");
  case TAG_F64:        return sym_1("f64");
  case TAG_FACT:       return sym_1("fact");
  case TAG_FN:         return sym_1("fn");
  case TAG_IDENT:      return sym_1("ident");
  case TAG_INTEGER:    return sym_1("integer");
  case TAG_SW:         return sym_1("sw");
  case TAG_S64:        return sym_1("s64");
  case TAG_S32:        return sym_1("s32");
  case TAG_S16:        return sym_1("s16");
  case TAG_S8:         return sym_1("s8");
  case TAG_U8:         return sym_1("u8");
  case TAG_U16:        return sym_1("u16");
  case TAG_U32:        return sym_1("u32");
  case TAG_U64:        return sym_1("u64");
  case TAG_UW:         return sym_1("uw");
  case TAG_LIST:       return sym_1("list");
  case TAG_PTAG:       return sym_1("ptag");
  case TAG_QUOTE:      return sym_1("quote");
  case TAG_STR:        return sym_1("str");
  case TAG_SYM:        return sym_1("sym");
  case TAG_TUPLE:      return sym_1("tuple");
  case TAG_VAR:        return sym_1("var");
  }
  assert(! "tag_type_to_sym: invalid tag type");
  errx(1, "tag_type_to_sym: invalid tag type: %d", tag_type);
  return NULL;
}

s_tag * tag_u8 (s_tag *tag, u8 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u8(tag, x);
}

s_tag * tag_u16 (s_tag *tag, u16 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u16(tag, x);
}

s_tag * tag_u32 (s_tag *tag, u32 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u32(tag, x);
}

s_tag * tag_u64 (s_tag *tag, u64 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u64(tag, x);
}

s_tag * tag_uw (s_tag *tag, uw x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_uw(tag, x);
}

s_tag * tag_var (s_tag *tag)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_var(tag);
}

s_tag * tag_void (s_tag *tag)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_void(tag);
}

bool tag_xor (const s_tag *a, const s_tag *b)
{
  s_tag f;
  tag_init_1(&f, "false");
  return (compare_tag(a, &f) != 0) != (compare_tag(b, &f) != 0);
}
