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
  s_tag tag = {0};
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
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.f32,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
#if HAVE_F80
      return tag_init_f80
        (dest, fmodl((f80) a->data.f32,
                     integer_to_f80(&b->data.integer)));
#else
      return tag_init_f64
        (dest, fmod((f64) a->data.f32,
                    integer_to_f64(&b->data.integer)));
#endif
    case TAG_S8:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.s8));
    case TAG_S16:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.s16));
    case TAG_S32:
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.s32));
    case TAG_S64:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f32,
                                       (f80) b->data.s64));
#else
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.s64));
#endif
    case TAG_SW:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f32,
                                       (f80) b->data.sw));
#else
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.sw));
#endif
    case TAG_U8:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.u8));
    case TAG_U16:
      return tag_init_f32(dest, fmodf(a->data.f32, (f32) b->data.u16));
    case TAG_U32:
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.u32));
    case TAG_U64:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f32,
                                       (f80) b->data.u64));
#else
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.u64));
#endif
    case TAG_UW:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f32,
                                       (f80) b->data.uw));
#else
      return tag_init_f64(dest, fmod((f64) a->data.f32,
                                     (f64) b->data.uw));
#endif
    default:
      goto ko;
    }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod(a->data.f64, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.f64,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
#if HAVE_F80
      return tag_init_f80
        (dest, fmodl((f80) a->data.f64, integer_to_f80(&b->data.integer)));
#else
      return tag_init_f64
        (dest, fmod(a->data.f64, integer_to_f64(&b->data.integer)));
#endif
    case TAG_S8:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s8));
    case TAG_S16:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s16));
    case TAG_S32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s32));
    case TAG_S64:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f64,
                                       (f80) b->data.s64));
#else
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.s64));
#endif
    case TAG_SW:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f64, (f80) b->data.sw));
#else
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.sw));
#endif
    case TAG_U8:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u8));
    case TAG_U16:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u16));
    case TAG_U32:
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u32));
    case TAG_U64:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f64,
                                       (f80) b->data.u64));
#else
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.u64));
#endif
    case TAG_UW:
#if HAVE_F80
      return tag_init_f80(dest, fmodl((f80) a->data.f64, (f80) b->data.uw));
#else
      return tag_init_f64(dest, fmod(a->data.f64, (f64) b->data.uw));
#endif
    default:
      goto ko;
    }

#if HAVE_F80
  case TAG_F80:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.f32));
    case TAG_F64:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.f64));
    case TAG_F80:
      return tag_init_f80(dest, fmodl(a->data.f80, b->data.f80));
    case TAG_INTEGER:
      return tag_init_f80
        (dest, fmodl(a->data.f80, integer_to_f80(&b->data.integer)));
    case TAG_S8:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.s8));
    case TAG_S16:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.s16));
    case TAG_S32:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.s32));
    case TAG_S64:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.s64));
    case TAG_SW:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.sw));
    case TAG_U8:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.u8));
    case TAG_U16:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.u16));
    case TAG_U32:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.u32));
    case TAG_U64:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.u64));
    case TAG_UW:
      return tag_init_f80(dest, fmodl(a->data.f80,
                                       (f80) b->data.uw));
    default:
      goto ko;
    }
#endif

  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod(integer_to_f64(&a->data.integer),
                                     (f64) b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod(integer_to_f64(&a->data.integer),
                                     b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80
        (dest, fmodl(integer_to_f80(&a->data.integer), b->data.f80));
#endif
    case TAG_INTEGER:
      tag.type = TAG_INTEGER;
      integer_mod(&a->data.integer, &b->data.integer,
                  &tag.data.integer);
      goto integer_reduce;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s8(&tag, integer_to_s8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s16(&tag, integer_to_s16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s32(&tag, integer_to_s32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_s64(&tag, integer_to_s64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_sw(&tag, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
     case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u8(&tag, integer_to_u8(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u16(&tag, integer_to_u16(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u32(&tag, integer_to_u32(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_u64(&tag, integer_to_u64(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      integer_mod(&a->data.integer, &tmp, &tmp2);
      tag_init_uw(&tag, integer_to_uw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.s8, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s8, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.s8,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s8(&tag, a->data.s8 % b->data.s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s8(&tag, a->data.s8 % b->data.s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s8(&tag, a->data.s8 % b->data.s32);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s8(&tag, a->data.s8 % b->data.s64);
      goto integer_reduce;
    case TAG_SW:
      tag_init_s8(&tag, a->data.s8 % b->data.sw);
      goto integer_reduce;
    case TAG_U8: {
      s16 i = a->data.s8 % (s16) b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(&tag, i);
      goto integer_reduce;
    }
    case TAG_U16: {
      s32 i = a->data.s8 % (s32) b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(&tag, i);
      goto integer_reduce;
    }
    case TAG_U32: {
      s64 i = a->data.s8 % (s64) b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(&tag, i);
      goto integer_reduce;
    }
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.s16, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s16, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.s16,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s8(&tag, a->data.s16 % b->data.s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s16(&tag, a->data.s16 % b->data.s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s16(&tag, a->data.s16 % b->data.s32);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s16(&tag, a->data.s16 % b->data.s64);
      goto integer_reduce;
    case TAG_SW:
      tag_init_s16(&tag, a->data.s16 % b->data.sw);
      goto integer_reduce;
    case TAG_U8: {
      s16 i = a->data.s16 % (s16) b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(&tag, i);
      goto integer_reduce;
    }
    case TAG_U16: {
      s32 i = a->data.s16 % (s32) b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(&tag, i);
      goto integer_reduce;
    }
    case TAG_U32: {
      s64 i = a->data.s16 % (s64) b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(&tag, i);
      goto integer_reduce;
    }
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.s32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s32, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.s32,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s8(&tag, a->data.s32 % b->data.s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s16(&tag, a->data.s32 % b->data.s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s32(&tag, a->data.s32 % b->data.s32);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s64(&tag, a->data.s32 % b->data.s64);
      goto integer_reduce;
    case TAG_SW:
      tag_init_sw(&tag, a->data.s32 % b->data.sw);
      goto integer_reduce;
    case TAG_U8: {
      s16 i = a->data.s32 % (s16) b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(&tag, i);
      goto integer_reduce;
    }
    case TAG_U16: {
      s32 i = a->data.s32 % (s32) b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(&tag, i);
      goto integer_reduce;
    }
    case TAG_U32: {
      s64 i = a->data.s32 % (s64) b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(&tag, i);
      goto integer_reduce;
    }
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.s64, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.s64, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.s64,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s8(&tag, a->data.s64 % b->data.s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s16(&tag, a->data.s64 % b->data.s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s32(&tag, a->data.s64 % b->data.s32);
      goto integer_reduce;
    case TAG_S64:
      tag_init_s64(&tag, a->data.s64 % b->data.s64);
      goto integer_reduce;
    case TAG_SW:
      tag_init_sw(&tag, a->data.s64 % b->data.sw);
      goto integer_reduce;
    case TAG_U8: {
      s16 i = a->data.s64 % b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(&tag, i);
      goto integer_reduce;
    }
    case TAG_U16: {
      s32 i = a->data.s64 % b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(&tag, i);
      goto integer_reduce;
    }
    case TAG_U32: {
      s64 i = a->data.s64 % b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(&tag, i);
      goto integer_reduce;
    }
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_SW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.sw, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.sw, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.sw,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.sw);
      integer_mod(&tmp, &b->data.integer, &tmp2);
      tag_init_sw(&tag, integer_to_sw(&tmp2));
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S8:
      tag_init_s8(&tag, a->data.sw % b->data.s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s16(&tag, a->data.sw % b->data.s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s32(&tag, a->data.sw % b->data.s32);
      goto integer_reduce;
    case TAG_S64:
      tag_init_sw(&tag, a->data.sw % b->data.s64);
      goto integer_reduce;
    case TAG_SW:
      tag_init_sw(&tag, a->data.sw % b->data.sw);
      goto integer_reduce;
    case TAG_U8: {
      s16 i = a->data.sw % b->data.u8;
      if (i < 0)
        i += b->data.u8;
      tag_init_u8(&tag, i);
      goto integer_reduce;
    }
    case TAG_U16: {
      s32 i = a->data.sw % b->data.u16;
      if (i < 0)
        i += b->data.u16;
      tag_init_u16(&tag, i);
      goto integer_reduce;
    }
    case TAG_U32: {
      s64 i = a->data.sw % b->data.u32;
      if (i < 0)
        i += b->data.u32;
      tag_init_u32(&tag, i);
      goto integer_reduce;
    }
    case TAG_U64:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_u64(&tmp2, b->data.u64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_uw(&tmp2, b->data.uw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.u8, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u8, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.u8,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8: {
      s16 i = a->data.u8 % b->data.s8;
      if (b->data.s8 < 0 && i > 0)
        i += b->data.s8;
      tag_init_s8(&tag, i);
      goto integer_reduce;
    }
    case TAG_S16: {
      s32 i = a->data.u8 % b->data.s16;
      if (b->data.s16 < 0 && i > 0)
        i += b->data.s16;
      tag_init_s16(&tag, i);
      goto integer_reduce;
    }
    case TAG_S32: {
      s64 i = a->data.u8 % b->data.s32;
      if (b->data.s32 < 0 && i > 0)
        i += b->data.s32;
      tag_init_s32(&tag, i);
      goto integer_reduce;
    }
    case TAG_S64:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_s64(&tmp2, b->data.s64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_u8(&tmp, a->data.u8);
      integer_init_sw(&tmp2, b->data.sw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u8(&tag, a->data.u8 % b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u8(&tag, a->data.u8 % b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u8(&tag, a->data.u8 % b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u8(&tag, a->data.u8 % b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u8(&tag, a->data.u8 % b->data.uw);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, fmodf((f32) a->data.u16, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u16, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.u16,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8: {
      s16 i = a->data.u16 % b->data.s8;
      if (b->data.s8 < 0 && i > 0)
        i += b->data.s8;
      tag_init_s8(&tag, i);
      goto integer_reduce;
    }
    case TAG_S16: {
      s32 i = a->data.u16 % b->data.s16;
      if (b->data.s16 < 0 && i > 0)
        i += b->data.s16;
      tag_init_s16(&tag, i);
      goto integer_reduce;
    }
    case TAG_S32: {
      s64 i = a->data.u16 % b->data.s32;
      if (b->data.s32 < 0 && i > 0)
        i += b->data.s32;
      tag_init_s32(&tag, i);
      goto integer_reduce;
    }
    case TAG_S64:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_s64(&tmp2, b->data.s64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_u16(&tmp, a->data.u16);
      integer_init_sw(&tmp2, b->data.sw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u8(&tag, a->data.u16 % b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u16(&tag, a->data.u16 % b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u16(&tag, a->data.u16 % b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u16(&tag, a->data.u16 % b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u16(&tag, a->data.u16 % b->data.uw);
      goto integer_reduce;
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.u32, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u32, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.u32,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8: {
      s64 i = a->data.u32 % (s64) b->data.s8;
      if (b->data.s8 < 0 && i > 0)
        i += b->data.s8;
      tag_init_s8(&tag, i);
      goto integer_reduce;
    }
    case TAG_S16: {
      s64 i = a->data.u32 % (s64) b->data.s16;
      if (b->data.s16 < 0 && i > 0)
        i += b->data.s16;
      tag_init_s16(&tag, i);
      goto integer_reduce;
    }
    case TAG_S32: {
      s64 i = a->data.u32 % (s64) b->data.s32;
      if (b->data.s32 < 0 && i > 0)
        i += b->data.s32;
      tag_init_s32(&tag, i);
      goto integer_reduce;
    }
    case TAG_S64:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_s64(&tmp2, b->data.s64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_u32(&tmp, a->data.u32);
      integer_init_sw(&tmp2, b->data.sw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u8(&tag, a->data.u32 % b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u16(&tag, a->data.u32 % b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u32(&tag, a->data.u32 % b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u32(&tag, a->data.u32 % b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_u32(&tag, a->data.u32 % b->data.uw);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.u64, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.u64, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.u64,
                                       b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_sw(&tmp2, b->data.sw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u8(&tag, a->data.u64 % b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u16(&tag, a->data.u64 % b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u32(&tag, a->data.u64 % b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_u64(&tag, a->data.u64 % b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_uw(&tag, a->data.u64 % b->data.uw);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_UW:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, fmod((f64) a->data.uw, b->data.f32));
    case TAG_F64:
      return tag_init_f64(dest, fmod((f64) a->data.uw, b->data.f64));
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, fmodl((f80) a->data.uw, b->data.f80));
#endif
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &b->data.integer, &tag.data.integer);
      integer_clean(&tmp);
      goto integer_reduce;
    case TAG_S8:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s8(&tmp2, b->data.s8);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S16:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s16(&tmp2, b->data.s16);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S32:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s32(&tmp2, b->data.s32);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_S64:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s64(&tmp2, b->data.s64);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_sw(&tmp2, b->data.sw);
      tag.type = TAG_INTEGER;
      integer_mod(&tmp, &tmp2, &tag.data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U8:
      tag_init_u8(&tag, a->data.uw % b->data.u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u16(&tag, a->data.uw % b->data.u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u32(&tag, a->data.uw % b->data.u32);
      goto integer_reduce;
    case TAG_U64:
      tag_init_uw(&tag, a->data.uw % b->data.u64);
      goto integer_reduce;
    case TAG_UW:
      tag_init_uw(&tag, a->data.uw % b->data.uw);
      goto integer_reduce;
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
 integer_reduce:
  if (! tag_integer_reduce(&tag, dest)) {
    tag_clean(&tag);
    return NULL;
  }
  tag_clean(&tag);
  return dest;
}
