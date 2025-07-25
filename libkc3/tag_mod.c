/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "tag.h"

s_tag * tag_mod (s_tag *a, s_tag *b, s_tag *dest)
{
  s_integer tmp = {0};
  s_integer tmp2 = {0};
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
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.f32,
                                       b->data.f128));
    case TAG_INTEGER:
      return tag_init_f128
        (dest, fmodl((f128) a->data.f32,
                     integer_to_f128(&b->data.integer)));
    case TAG_S8:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.s8));
    case TAG_S16:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.s16));
    case TAG_S32:
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.s32));
    case TAG_S64:
      return tag_init_f128(dest, fmodl((f128) a->data.f32,
                                       (f128) b->data.s64));
    case TAG_SW:
      return tag_init_f128(dest, fmodl((f128) a->data.f32,
                                       (f128) b->data.sw));
    case TAG_U8:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.u8));
    case TAG_U16:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.u16));
    case TAG_U32:
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.u32));
    case TAG_U64:
      return tag_init_f128(dest, fmodl((f128) a->data.f32,
                                       (f128) b->data.u64));
    case TAG_UW:
      return tag_init_f128(dest, fmodl((f128) a->data.f32,
                                       (f128) b->data.uw));
    default:
      goto ko;
    }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod(a->data.f64, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.f64,
                                       b->data.f128));
    case TAG_INTEGER:
      return tag_init_f128
        (dest, fmod(a->data.f64, integer_to_f128(&b->data.integer)));
    case TAG_S8:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s8));
    case TAG_S16:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s16));
    case TAG_S32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s32));
    case TAG_S64:
      return tag_init_f128(dest, fmodl(a->data.f64,
                                       (f128) b->data.s64));
    case TAG_SW:
      return tag_init_f128(dest, fmodl(a->data.f64, (f128) b->data.sw));
    case TAG_U8:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u8));
    case TAG_U16:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u16));
    case TAG_U32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u32));
    case TAG_U64:
      return tag_init_f128(dest, fmodl(a->data.f64,
                                       (f128) b->data.u64));
    case TAG_UW:
      return tag_init_f128(dest, fmodl(a->data.f64, (f128) b->data.uw));
    default:
      goto ko;
    }
  case TAG_F128:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.f32));
    case TAG_F64:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl(a->data.f128, b->data.f128));
    case TAG_INTEGER:
      return tag_init_f128
        (dest, fmodl(a->data.f128, integer_to_f128(&b->data.integer)));
    case TAG_S8:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.s8));
    case TAG_S16:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.s16));
    case TAG_S32:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.s32));
    case TAG_S64:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.s64));
    case TAG_SW:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.sw));
    case TAG_U8:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.u8));
    case TAG_U16:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.u16));
    case TAG_U32:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.u32));
    case TAG_U64:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.u64));
    case TAG_UW:
      return tag_init_f128(dest, fmodl(a->data.f128,
                                       (f128) b->data.uw));
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
    case TAG_F128:
      return tag_init_f128
        (dest, fmodl(integer_to_f128(&a->data.integer), b->data.f128));
    case TAG_INTEGER:
      dest->type = TAG_INTEGER;
      integer_mod(&a->data.integer, &b->data.integer,
                  &dest->data.integer);
      return tag_integer_reduce(dest);
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s8(dest, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s16(dest, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s32(dest, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s64(dest, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_sw(dest, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
     case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u8(dest, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u16(dest, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u32(dest, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u64(dest, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_uw(dest, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.s8, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s8, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.s8,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8:
      tag_init_s8(dest, a->data.s8 % b->data.s8);
      return tag_integer_reduce(dest);
    case TAG_S16:
      tag_init_s8(dest, a->data.s8 % b->data.s16);
      return tag_integer_reduce(dest);
    case TAG_S32:
      tag_init_s8(dest, a->data.s8 % b->data.s32);
      return tag_integer_reduce(dest);
    case TAG_S64:
      tag_init_s8(dest, a->data.s8 % b->data.s64);
      return tag_integer_reduce(dest);
    case TAG_SW:
      tag_init_s8(dest, a->data.s8 % b->data.sw);
      return tag_integer_reduce(dest);
    case TAG_U8: {
      s16 i = a->data.s8 % (s16) b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U16: {
      s32 i = a->data.s8 % (s32) b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U32: {
      s64 i = a->data.s8 % (s64) b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_UW:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.s16, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s16, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.s16,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8:
      tag_init_s8(dest, a->data.s16 % b->data.s8);
      return tag_integer_reduce(dest);
    case TAG_S16:
      tag_init_s16(dest, a->data.s16 % b->data.s16);
      return tag_integer_reduce(dest);
    case TAG_S32:
      tag_init_s16(dest, a->data.s16 % b->data.s32);
      return tag_integer_reduce(dest);
    case TAG_S64:
      tag_init_s16(dest, a->data.s16 % b->data.s64);
      return tag_integer_reduce(dest);
    case TAG_SW:
      tag_init_s16(dest, a->data.s16 % b->data.sw);
      return tag_integer_reduce(dest);
    case TAG_U8: {
      s16 i = a->data.s16 % (s16) b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U16: {
      s32 i = a->data.s16 % (s32) b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U32: {
      s64 i = a->data.s16 % (s64) b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_UW:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.s32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s32, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.s32,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8:
      tag_init_s8(dest, a->data.s32 % b->data.s8);
      return tag_integer_reduce(dest);
    case TAG_S16:
      tag_init_s16(dest, a->data.s32 % b->data.s16);
      return tag_integer_reduce(dest);
    case TAG_S32:
      tag_init_s32(dest, a->data.s32 % b->data.s32);
      return tag_integer_reduce(dest);
    case TAG_S64:
      tag_init_s64(dest, a->data.s32 % b->data.s64);
      return tag_integer_reduce(dest);
    case TAG_SW:
      tag_init_sw(dest, a->data.s32 % b->data.sw);
      return tag_integer_reduce(dest);
    case TAG_U8: {
      s16 i = a->data.s32 % (s16) b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U16: {
      s32 i = a->data.s32 % (s32) b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U32: {
      s64 i = a->data.s32 % (s64) b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_UW:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.s64, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s64, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.s64,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8:
      tag_init_s8(dest, a->data.s64 % b->data.s8);
      return tag_integer_reduce(dest);
    case TAG_S16:
      tag_init_s16(dest, a->data.s64 % b->data.s16);
      return tag_integer_reduce(dest);
    case TAG_S32:
      tag_init_s32(dest, a->data.s64 % b->data.s32);
      return tag_integer_reduce(dest);
    case TAG_S64:
      tag_init_s64(dest, a->data.s64 % b->data.s64);
      return tag_integer_reduce(dest);
    case TAG_SW:
      tag_init_sw(dest, a->data.s64 % b->data.sw);
      return tag_integer_reduce(dest);
    case TAG_U8: {
      s16 i = a->data.s64 % b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U16: {
      s32 i = a->data.s64 % b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U32: {
      s64 i = a->data.s64 % b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_UW:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_SW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.sw, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.sw, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.sw,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_mod(&tmp, &b->data.integer, &tmp2);
      tag_init_sw(dest, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S8:
      tag_init_s8(dest, a->data.sw % b->data.s8);
      return tag_integer_reduce(dest);
    case TAG_S16:
      tag_init_s16(dest, a->data.sw % b->data.s16);
      return tag_integer_reduce(dest);
    case TAG_S32:
      tag_init_s32(dest, a->data.sw % b->data.s32);
      return tag_integer_reduce(dest);
    case TAG_S64:
      tag_init_sw(dest, a->data.sw % b->data.s64);
      return tag_integer_reduce(dest);
    case TAG_SW:
      tag_init_sw(dest, a->data.sw % b->data.sw);
      return tag_integer_reduce(dest);
    case TAG_U8: {
      s16 i = a->data.sw % b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U16: {
      s32 i = a->data.sw % b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U32: {
      s64 i = a->data.sw % b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_U64:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_u64(&tmp2, b->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_UW:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_uw(&tmp2, b->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.u8, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u8, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.u8,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8: {
      s16 i = a->data.u8 % b->data.s8;
      if (b->data.s8 < 0 && i > 0)
        i += b->data.s8;
      tag_init_s8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S16: {
      s32 i = a->data.u8 % b->data.s16;
      if (b->data.s16 < 0 && i > 0)
        i += b->data.s16;
      tag_init_s16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S32: {
      s64 i = a->data.u8 % b->data.s32;
      if (b->data.s32 < 0 && i > 0)
        i += b->data.s32;
      tag_init_s32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S64:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_SW:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U8:
      tag_init_u8(dest, a->data.u8 % b->data.u8);
      return tag_integer_reduce(dest);
    case TAG_U16:
      tag_init_u8(dest, a->data.u8 % b->data.u16);
      return tag_integer_reduce(dest);
    case TAG_U32:
      tag_init_u8(dest, a->data.u8 % b->data.u32);
      return tag_integer_reduce(dest);
    case TAG_U64:
      tag_init_u8(dest, a->data.u8 % b->data.u64);
      return tag_integer_reduce(dest);
    case TAG_UW:
      tag_init_u8(dest, a->data.u8 % b->data.uw);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.u16, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u16, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.u16,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8: {
      s16 i = a->data.u16 % b->data.s8;
      if (b->data.s8 < 0 && i > 0)
        i += b->data.s8;
      tag_init_s8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S16: {
      s32 i = a->data.u16 % b->data.s16;
      if (b->data.s16 < 0 && i > 0)
        i += b->data.s16;
      tag_init_s16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S32: {
      s64 i = a->data.u16 % b->data.s32;
      if (b->data.s32 < 0 && i > 0)
        i += b->data.s32;
      tag_init_s32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S64:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_SW:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U8:
      tag_init_u8(dest, a->data.u16 % b->data.u8);
      return tag_integer_reduce(dest);
    case TAG_U16:
      tag_init_u16(dest, a->data.u16 % b->data.u16);
      return tag_integer_reduce(dest);
    case TAG_U32:
      tag_init_u16(dest, a->data.u16 % b->data.u32);
      return tag_integer_reduce(dest);
    case TAG_U64:
      tag_init_u16(dest, a->data.u16 % b->data.u64);
      return tag_integer_reduce(dest);
    case TAG_UW:
      tag_init_u16(dest, a->data.u16 % b->data.uw);
      return tag_integer_reduce(dest);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.u32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u32, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.u32,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8: {
      s64 i = a->data.u32 % (s64) b->data.s8;
      if (b->data.s8 < 0 && i > 0)
        i += b->data.s8;
      tag_init_s8(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S16: {
      s64 i = a->data.u32 % (s64) b->data.s16;
      if (b->data.s16 < 0 && i > 0)
        i += b->data.s16;
      tag_init_s16(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S32: {
      s64 i = a->data.u32 % (s64) b->data.s32;
      if (b->data.s32 < 0 && i > 0)
        i += b->data.s32;
      tag_init_s32(dest, i);
      return tag_integer_reduce(dest);
    }
    case TAG_S64:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_SW:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U8:
      tag_init_u8(dest, a->data.u32 % b->data.u8);
      return tag_integer_reduce(dest);
    case TAG_U16:
      tag_init_u16(dest, a->data.u32 % b->data.u16);
      return tag_integer_reduce(dest);
    case TAG_U32:
      tag_init_u32(dest, a->data.u32 % b->data.u32);
      return tag_integer_reduce(dest);
    case TAG_U64:
      tag_init_u32(dest, a->data.u32 % b->data.u64);
      return tag_integer_reduce(dest);
    case TAG_UW:
      tag_init_u32(dest, a->data.u32 % b->data.uw);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.u64, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u64, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.u64,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_SW:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U8:
      tag_init_u8(dest, a->data.u64 % b->data.u8);
      return tag_integer_reduce(dest);
    case TAG_U16:
      tag_init_u16(dest, a->data.u64 % b->data.u16);
      return tag_integer_reduce(dest);
    case TAG_U32:
      tag_init_u32(dest, a->data.u64 % b->data.u32);
      return tag_integer_reduce(dest);
    case TAG_U64:
      tag_init_u64(dest, a->data.u64 % b->data.u64);
      return tag_integer_reduce(dest);
    case TAG_UW:
      tag_init_uw(dest, a->data.u64 % b->data.uw);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  case TAG_UW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.uw, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.uw, b->data.f64));
    case TAG_F128:
      return tag_init_f128(dest, fmodl((f128) a->data.uw,
                                       b->data.f128));
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &dest->data.integer);
      integer_clean(&tmp);
      return tag_integer_reduce(dest);
    case TAG_S8:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s8(&tmp2, b->data.s8);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S16:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s16(&tmp2, b->data.s16);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S32:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s32(&tmp2, b->data.s32);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_S64:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s64(&tmp2, b->data.s64);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_SW:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_sw(&tmp2, b->data.sw);
      dest->type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &dest->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return tag_integer_reduce(dest);
    case TAG_U8:
      tag_init_u8(dest, a->data.uw % b->data.u8);
      return tag_integer_reduce(dest);
    case TAG_U16:
      tag_init_u16(dest, a->data.uw % b->data.u16);
      return tag_integer_reduce(dest);
    case TAG_U32:
      tag_init_u32(dest, a->data.uw % b->data.u32);
      return tag_integer_reduce(dest);
    case TAG_U64:
      tag_init_uw(dest, a->data.uw % b->data.u64);
      return tag_integer_reduce(dest);
    case TAG_UW:
      tag_init_uw(dest, a->data.uw % b->data.uw);
      return tag_integer_reduce(dest);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  err_write_1("tag_mod: invalid tag type: ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" mod ");
  err_puts(tag_type_to_string(b->type));
  assert(! "tag_mod: invalid tag type");
  return NULL;
}
