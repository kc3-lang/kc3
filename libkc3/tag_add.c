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
#include "complex.h"
#include "integer.h"
#include "plist.h"
#include "ratio.h"
#include "tag.h"

s_tag * tag_add (s_tag *a, s_tag *b, s_tag *dest)
{
  s_complex c = {0};
  p_list list;
  s_tag tag = {0};
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  s_ratio r = {0};
  assert(a);
  assert(b);
  assert(dest);
  a = tag_resolve_cow(a);
  b = tag_resolve_cow(b);
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_f32(&c, a->data.td_f32);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, a->data.td_f32 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_f32 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_f32 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      return tag_init_f32(dest, a->data.td_f32 +
                          integer_to_f32(&a->data.td_integer));
    case TAG_RATIO:
      ratio_init_f32(&r, a->data.td_f32);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_s64);
    case TAG_SW:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_sw);
    case TAG_U8:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_u64);
    case TAG_UW:
      return tag_init_f32(dest, a->data.td_f32 + (f32) b->data.td_uw);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_f64(&c, a->data.td_f64);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.td_f64 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_f64 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      return tag_init_f64(dest, a->data.td_f64 +
                          integer_to_f64(&a->data.td_integer));
    case TAG_RATIO:
      ratio_init_f64(&r, a->data.td_f64);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_s64);
    case TAG_SW:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_sw);
    case TAG_U8:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_u64);
    case TAG_UW:
      return tag_init_f64(dest, a->data.td_f64 + (f64) b->data.td_uw);
    default:
      goto ko;
  }
#if HAVE_F80
  case TAG_F80:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_f80(&c, a->data.td_f80);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_f32);
    case TAG_F64:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_f64);
    case TAG_F80:
      return tag_init_f80(dest, a->data.td_f80 + b->data.td_f80);
    case TAG_INTEGER:
      return tag_init_f80(dest, a->data.td_f80 +
                          integer_to_f80(&a->data.td_integer));
    case TAG_RATIO:
      ratio_init_f80(&r, a->data.td_f80);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_s8);
    case TAG_S16:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_s16);
    case TAG_S32:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_s32);
    case TAG_S64:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_s64);
    case TAG_SW:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_sw);
    case TAG_U8:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_u8);
    case TAG_U16:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_u16);
    case TAG_U32:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_u32);
    case TAG_U64:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_u64);
    case TAG_UW:
      return tag_init_f80(dest, a->data.td_f80 + (f80) b->data.td_uw);
    default:
      goto ko;
  }
#endif
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_PCOMPLEX:
      if (! complex_init_integer(&c, &a->data.td_integer))
        return NULL;
      if (! tag_init_pcomplex(dest,
                              complex_new_add(&c, b->data.td_pcomplex))) {
        complex_clean(&c);
        return NULL;
      }
      complex_clean(&c);
      return dest;
    case TAG_F32:
      return tag_init_f32(dest, (f32) integer_to_f64(&a->data.td_integer) +
                          b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, integer_to_f64(&a->data.td_integer) +
                          b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, integer_to_f80(&a->data.td_integer) +
                          b->data.td_f80);
#endif
    case TAG_INTEGER:
      if (! integer_add(&a->data.td_integer, &b->data.td_integer,
                        &tmp))
        return NULL;
      integer_reduce(&tmp, dest);
      integer_clean(&tmp);
      return dest;
    case TAG_RATIO:
      ratio_init_integer(&r, &a->data.td_integer);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      integer_init_s8(&tmp, b->data.td_s8);
      goto integer_add;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.td_s16);
      goto integer_add;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.td_s32);
      goto integer_add;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.td_s64);
      goto integer_add;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.td_sw);
      goto integer_add;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.td_u8);
      goto integer_add;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.td_u16);
      goto integer_add;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.td_u32);
      goto integer_add;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.td_u64);
      goto integer_add;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.td_uw);
      goto integer_add;
    default:
      goto ko;
    }
  case TAG_PCOMPLEX:
    switch (b->type) {
    case TAG_F32:
      complex_init_f32(&c, b->data.td_f32);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_F64:
      complex_init_f64(&c, b->data.td_f64);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
#if HAVE_F80
    case TAG_F80:
      complex_init_f80(&c, b->data.td_f80);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
#endif
    case TAG_INTEGER:
      if (! complex_init_integer(&c, &b->data.td_integer))
        return NULL;
      if (! tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                    &c))) {
                                                   
        complex_clean(&c);
        return NULL;
      }
      complex_clean(&c);
      return dest;
    case TAG_PCOMPLEX:
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     b->data.td_pcomplex));
    case TAG_RATIO:
      if (! complex_init_ratio(&c, &b->data.td_ratio))
        return NULL;
      if (! tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                    &c))) {
                                                   
        complex_clean(&c);
        return NULL;
      }
      complex_clean(&c);
      return dest;
    case TAG_S8:
      complex_init_s8(&c, b->data.td_s8);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_S16:
      complex_init_s16(&c, b->data.td_s16);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_S32:
      complex_init_s32(&c, b->data.td_s32);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_S64:
      complex_init_s64(&c, b->data.td_s64);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_SW:
      complex_init_sw(&c, b->data.td_sw);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_U8:
      complex_init_u8(&c, b->data.td_u8);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_U16:
      complex_init_u16(&c, b->data.td_u16);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_U32:
      complex_init_u32(&c, b->data.td_u32);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_U64:
      complex_init_u64(&c, b->data.td_u64);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    case TAG_UW:
      complex_init_uw(&c, b->data.td_uw);
      return tag_init_pcomplex(dest, complex_new_add(a->data.td_pcomplex,
                                                     &c));
    default:
      break;
    }
    break;
  case TAG_PLIST:
    switch (b->type) {
    case TAG_PLIST:
      if (! plist_init_append(&list, &a->data.td_plist, &b->data.td_plist))
        return NULL;
      return tag_init_plist(dest, list);
    case TAG_VOID:
      return tag_init_copy(dest, a);
    default:
      break;
    }
    break;
  case TAG_RATIO:
    switch (b->type) {
    case TAG_PCOMPLEX:
      if (! complex_init_ratio(&c, &a->data.td_ratio))
        return NULL;
      if (! tag_init_pcomplex(dest,
                              complex_new_add(&c, b->data.td_pcomplex))) {
        complex_clean(&c);
        return NULL;
      }
      complex_clean(&c);
      return dest;
    case TAG_F32:
      ratio_init_f32(&r, b->data.td_f32);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_F64:
      ratio_init_f64(&r, b->data.td_f64);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
#if HAVE_F80
    case TAG_F80:
      ratio_init_f80(&r, b->data.td_f80);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
#endif
    case TAG_INTEGER:
      ratio_init_integer(&r, &b->data.td_integer);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_RATIO:
      if (! ratio_add(&a->data.td_ratio, &b->data.td_ratio,
                      &dest->data.td_ratio))
        return NULL;
      dest->type = TAG_RATIO;
      return dest;
    case TAG_S8:
      ratio_init_s8(&r, b->data.td_s8);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S16:
      ratio_init_s16(&r, b->data.td_s16);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S32:
      ratio_init_s32(&r, b->data.td_s32);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S64:
      ratio_init_s64(&r, b->data.td_s64);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_SW:
      ratio_init_sw(&r, b->data.td_sw);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_U8:
      ratio_init_u8(&r, b->data.td_u8);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_U16:
      ratio_init_u16(&r, b->data.td_u16);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_U32:
      ratio_init_u32(&r, b->data.td_u32);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_U64:
      ratio_init_u64(&r, b->data.td_u64);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_UW:
      ratio_init_uw(&r, b->data.td_uw);
      dest->type = TAG_RATIO;
      ratio_add(&a->data.td_ratio, &r, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    default:
      goto ko;
    }
  case TAG_S8:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_s8(&c, a->data.td_s8);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_s8 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_s8 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_s8 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.td_s8);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_s8(&r, a->data.td_s8);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      tag_init_s16(&tag, (s16) a->data.td_s8 + (s16) b->data.td_s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s32(&tag, (s32) a->data.td_s8 + (s32) b->data.td_s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, (s64) a->data.td_s8 + (s64) b->data.td_s32);
      goto integer_reduce;
    case TAG_S64:
      integer_init_s8(&tmp, a->data.td_s8);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_s8(&tmp, a->data.td_s8);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      tag_init_s16(&tag, (s16) a->data.td_s8 + (s16) b->data.td_u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s32(&tag, (s32) a->data.td_s8 + (s32) b->data.td_u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, (s64) a->data.td_s8 + (s64) b->data.td_u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s8(&tmp, a->data.td_s8);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_s8(&tmp, a->data.td_s8);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_s16(&c, a->data.td_s16);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_s16 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_s16 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_s16 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.td_s16);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_s16(&r, a->data.td_s16);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      tag_init_s32(&tag, (s32) a->data.td_s16 + (s32) b->data.td_s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s32(&tag, (s32) a->data.td_s16 + (s32) b->data.td_s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, (s64) a->data.td_s16 + (s64) b->data.td_s32);
      goto integer_reduce;
    case TAG_S64:
      integer_init_s16(&tmp, a->data.td_s16);
      integer_init_s64(&tmp2, b->data.td_s64);
      tag.type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &tag.data.td_integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_SW:
      integer_init_s16(&tmp, a->data.td_s16);
      integer_init_sw(&tmp2, b->data.td_sw);
      tag.type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &tag.data.td_integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_U8:
      tag_init_s32(&tag, (s32) a->data.td_s16 + (s32) b->data.td_u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s32(&tag, (s32) a->data.td_s16 + (s32) b->data.td_u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, (s64) a->data.td_s16 + (s64) b->data.td_u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s16(&tmp, a->data.td_s16);
      integer_init_u64(&tmp2, b->data.td_u64);
      tag.type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &tag.data.td_integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    case TAG_UW:
      integer_init_s16(&tmp, a->data.td_s16);
      integer_init_uw(&tmp2, b->data.td_uw);
      tag.type = TAG_INTEGER;
      integer_add(&tmp, &tmp2, &tag.data.td_integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      goto integer_reduce;
    default:
      goto ko;
    }
  case TAG_S32:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_s32(&c, a->data.td_s32);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_s32 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_s32 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_s32 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.td_s32);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_s32(&r, a->data.td_s32);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      tag_init_s64(&tag, (s64) a->data.td_s32 + (s64) b->data.td_s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s64(&tag, (s64) a->data.td_s32 + (s64) b->data.td_s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, (s64) a->data.td_s32 + (s64) b->data.td_s32);
      goto integer_reduce;
    case TAG_S64:
      integer_init_s32(&tmp, a->data.td_s32);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_s32(&tmp, a->data.td_s32);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      tag_init_s64(&tag, (s64) a->data.td_s32 + (s64) b->data.td_u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_s64(&tag, (s64) a->data.td_s32 + (s64) b->data.td_u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_s64(&tag, (s64) a->data.td_s32 + (s64) b->data.td_u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_s32(&tmp, a->data.td_s32);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_s32(&tmp, a->data.td_s32);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_s64(&c, a->data.td_s64);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_s64 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_s64 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_s64 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.td_s64);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_s64(&r, a->data.td_s64);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_s8(&tmp2, b->data.td_s8);
      goto integer_add_a_b;
    case TAG_S16:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_s16(&tmp2, b->data.td_s16);
      goto integer_add_a_b;
    case TAG_S32:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_s32(&tmp2, b->data.td_s32);
      goto integer_add_a_b;
    case TAG_S64:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_u8(&tmp2, b->data.td_u8);
      goto integer_add_a_b;
    case TAG_U16:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_u16(&tmp2, b->data.td_u16);
      goto integer_add_a_b;
    case TAG_U32:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_u32(&tmp2, b->data.td_u32);
      goto integer_add_a_b;
    case TAG_U64:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_s64(&tmp, a->data.td_s64);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_SW:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_sw(&c, a->data.td_sw);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_sw + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_sw + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_sw + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_sw(&tmp, a->data.td_sw);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_sw(&r, a->data.td_sw);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_s8(&tmp2, b->data.td_s8);
      goto integer_add_a_b;
    case TAG_S16:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_s16(&tmp2, b->data.td_s16);
      goto integer_add_a_b;
    case TAG_S32:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_s32(&tmp2, b->data.td_s32);
      goto integer_add_a_b;
    case TAG_S64:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_u8(&tmp2, b->data.td_u8);
      goto integer_add_a_b;
    case TAG_U16:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_u16(&tmp2, b->data.td_u16);
      goto integer_add_a_b;
    case TAG_U32:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_u32(&tmp2, b->data.td_u32);
      goto integer_add_a_b;
    case TAG_U64:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_sw(&tmp, a->data.td_sw);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_u8(&c, a->data.td_u8);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_u8 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_u8 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_u8 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.td_u8);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_u8(&r, a->data.td_u8);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      tag_init_s16(&tag, (s16) a->data.td_u8 + (s16) b->data.td_s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s32(&tag, (s32) a->data.td_u8 + (s32) b->data.td_s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, (s64) a->data.td_u8 + (s64) b->data.td_s32);
      goto integer_reduce;
    case TAG_S64:
      integer_init_u8(&tmp, a->data.td_u8);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_u8(&tmp, a->data.td_u8);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      tag_init_u16(&tag, (u16) a->data.td_u8 + (u16) b->data.td_u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u32(&tag, (u32) a->data.td_u8 + (u32) b->data.td_u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u64(&tag, (u64) a->data.td_u8 + (u64) b->data.td_u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_u8(&tmp, a->data.td_u8);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_u8(&tmp, a->data.td_u8);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_u16(&c, a->data.td_u16);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_u16 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_u16 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_u16 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.td_u16);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_u16(&r, a->data.td_u16);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      tag_init_s32(&tag, (s32) a->data.td_u16 + (s32) b->data.td_s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s32(&tag, (s32) a->data.td_u16 + (s32) b->data.td_s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, (s64) a->data.td_u16 + (s64) b->data.td_s32);
      goto integer_reduce;
    case TAG_S64:
      integer_init_u16(&tmp, a->data.td_u16);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_u16(&tmp, a->data.td_u16);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      tag_init_u32(&tag, (u32) a->data.td_u16 + (u32) b->data.td_u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u32(&tag, (u32) a->data.td_u16 + (u32) b->data.td_u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u64(&tag, (u64) a->data.td_u16 + (u64) b->data.td_u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_u32(&tmp, (u32) a->data.td_u16);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_u16(&tmp, a->data.td_u16);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_u32(&c, a->data.td_u32);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_u32 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_u32 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_u32 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.td_u32);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_u32(&r, a->data.td_u32);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      tag_init_s64(&tag, (s64) a->data.td_u32 + (s64) b->data.td_s8);
      goto integer_reduce;
    case TAG_S16:
      tag_init_s64(&tag, (s64) a->data.td_u32 + (s64) b->data.td_s16);
      goto integer_reduce;
    case TAG_S32:
      tag_init_s64(&tag, (s64) a->data.td_u32 + (s64) b->data.td_s32);
      goto integer_reduce;
    case TAG_S64:
      integer_init_u32(&tmp, a->data.td_u32);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_u32(&tmp, a->data.td_u32);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      tag_init_u64(&tag, (u64) a->data.td_u32 + (u64) b->data.td_u8);
      goto integer_reduce;
    case TAG_U16:
      tag_init_u64(&tag, (u64) a->data.td_u32 + (u64) b->data.td_u16);
      goto integer_reduce;
    case TAG_U32:
      tag_init_u64(&tag, (u64) a->data.td_u32 + (u64) b->data.td_u32);
      goto integer_reduce;
    case TAG_U64:
      integer_init_u32(&tmp, a->data.td_u32);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_u32(&tmp, a->data.td_u32);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_u64(&c, a->data.td_u64);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_u64 + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_u64 + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_u64 + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.td_u64);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_u64(&r, a->data.td_u64);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_s8(&tmp2, b->data.td_s8);
      goto integer_add_a_b;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_s16(&tmp2, b->data.td_s16);
      goto integer_add_a_b;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_s32(&tmp2, b->data.td_s32);
      goto integer_add_a_b;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_u8(&tmp2, b->data.td_u8);
      goto integer_add_a_b;
    case TAG_U16:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_u16(&tmp2, b->data.td_u16);
      goto integer_add_a_b;
    case TAG_U32:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_u32(&tmp2, b->data.td_u32);
      goto integer_add_a_b;
    case TAG_U64:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_u64(&tmp, a->data.td_u64);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
    }
  case TAG_UW:
    switch (b->type) {
    case TAG_PCOMPLEX:
      complex_init_uw(&c, a->data.td_uw);
      return tag_init_pcomplex(dest, complex_new_add(&c,
                                                     b->data.td_pcomplex));
    case TAG_F32:
      return tag_init_f32(dest, (f32) a->data.td_uw + b->data.td_f32);
    case TAG_F64:
      return tag_init_f64(dest, (f64) a->data.td_uw + b->data.td_f64);
#if HAVE_F80
    case TAG_F80:
      return tag_init_f80(dest, (f80) a->data.td_uw + b->data.td_f80);
#endif
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.td_uw);
      goto integer_add_b;
    case TAG_RATIO:
      ratio_init_uw(&r, a->data.td_uw);
      dest->type = TAG_RATIO;
      ratio_add(&r, &b->data.td_ratio, &dest->data.td_ratio);
      ratio_clean(&r);
      return dest;
    case TAG_S8:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_s8(&tmp2, b->data.td_s8);
      goto integer_add_a_b;
    case TAG_S16:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_s16(&tmp2, b->data.td_s16);
      goto integer_add_a_b;
    case TAG_S32:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_s32(&tmp2, b->data.td_s32);
      goto integer_add_a_b;
    case TAG_S64:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_s64(&tmp2, b->data.td_s64);
      goto integer_add_a_b;
    case TAG_SW:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_sw(&tmp2, b->data.td_sw);
      goto integer_add_a_b;
    case TAG_U8:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_u8(&tmp2, b->data.td_u8);
      goto integer_add_a_b;
    case TAG_U16:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_u16(&tmp2, b->data.td_u16);
      goto integer_add_a_b;
    case TAG_U32:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_u32(&tmp2, b->data.td_u32);
      goto integer_add_a_b;
    case TAG_U64:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_u64(&tmp2, b->data.td_u64);
      goto integer_add_a_b;
    case TAG_UW:
      integer_init_uw(&tmp, a->data.td_uw);
      integer_init_uw(&tmp2, b->data.td_uw);
      goto integer_add_a_b;
    default:
      goto ko;
    }
  case TAG_STR:
    switch (b->type) {
    case TAG_STR:
      return tag_init_str_concatenate(dest, &a->data.td_str, &b->data.td_str);
    default:
      goto ko;
    }
  case TAG_TIME:
    switch (b->type) {
    case TAG_TIME:
      return tag_init_time_add(dest, &a->data.td_time, &b->data.td_time);
    default:
      goto ko;
    }
  case TAG_VOID:
    switch (b->type) {
    case TAG_PLIST:
      return tag_init_copy(dest, b);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  err_write_1("tag_add: invalid tag type: ");
  err_write_1(tag_type_to_string(a->type));
  err_write_1(" + ");
  err_puts(tag_type_to_string(b->type));
  err_stacktrace();
  assert(! "tag_add: invalid tag type");
  return NULL;
 integer_add:
  integer_add(&a->data.td_integer, &tmp, &tmp2);
  integer_clean(&tmp);
  if (! integer_reduce(&tmp2, dest)) {
    integer_clean(&tmp2);
    return NULL;
  }
  integer_clean(&tmp2);
  return dest;
 integer_add_b:
  integer_add(&tmp, &b->data.td_integer, &tmp2);
  integer_clean(&tmp);
  if (! integer_reduce(&tmp2, dest)) {
    integer_clean(&tmp2);
    return NULL;
  }
  integer_clean(&tmp2);
  return dest;
 integer_add_a_b:
  tag.type = TAG_INTEGER;
  integer_add(&tmp, &tmp2, &tag.data.td_integer);
  integer_clean(&tmp);
  integer_clean(&tmp2);
 integer_reduce:
  if (! tag_integer_reduce(&tag, dest)) {
    integer_clean(&tag.data.td_integer);
    return NULL;
  }
  integer_clean(&tag.data.td_integer);
  return dest;
}
