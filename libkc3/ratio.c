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
#include <string.h>
#include <stdlib.h>
#include "buf.h"
#include "buf_parse.h"
#include "buf_inspect.h"
#include "compare.h"
#include "integer.h"
#include "ratio.h"

s_ratio * ratio_add (const s_ratio *a, const s_ratio *b, s_ratio *dest)
{
  s_ratio tmp = {0};
  s_integer i = {0};
  s_integer j = {0};
  assert(a);
  assert(b);
  assert(dest);
  assert(integer_is_positive(&a->denominator));
  assert(integer_is_positive(&b->denominator));
  if (! integer_mul(&a->numerator, &b->denominator, &i)) {
    ratio_clean(&tmp);
    return NULL;
  }
  if (! integer_mul(&b->numerator, &a->denominator, &j)) {
    integer_clean(&i);
    ratio_clean(&tmp);
    return NULL;
  }
  if (! integer_add(&i, &j, &tmp.numerator)) {
    integer_clean(&i);
    integer_clean(&j);
    ratio_clean(&tmp);
    return NULL;
  }
  integer_clean(&i);
  integer_clean(&j);
  if (! integer_mul(&a->denominator, &b->denominator,
                    &tmp.denominator)) {
    ratio_clean(&tmp);
    return NULL;
  }
  if (! ratio_simplify(&tmp, dest)) {
    ratio_clean(&tmp);
    return NULL;
  }
  ratio_clean(&tmp);
  return dest;
}

void ratio_clean (s_ratio *r)
{
  assert(r);
  integer_clean(&r->numerator);
  integer_clean(&r->denominator);
}

s_ratio * ratio_div (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest)
{
  s_ratio tmp = {0};
  s_ratio tmp2 = {0};
  assert(a);
  assert(b);
  assert(dest);
  assert(! integer_is_zero(&b->numerator));
  assert(integer_is_positive(&a->denominator));
  assert(integer_is_positive(&b->denominator));
  if (! integer_mul(&a->numerator, &b->denominator, &tmp.numerator))
    return NULL;
  if (! integer_mul(&a->denominator, &b->numerator, &tmp.denominator)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  if (integer_is_negative(&tmp.denominator)) {
    tmp2 = tmp;
    if (! integer_neg(&tmp2.numerator, &tmp.numerator)) {
      ratio_clean(&tmp2);
      return NULL;
    }
    if (! integer_neg(&tmp2.denominator, &tmp.denominator)) {
      integer_clean(&tmp.numerator);
      ratio_clean(&tmp2);
      return NULL;
    }
    ratio_clean(&tmp2);
  }
  if (! ratio_simplify(&tmp, dest)) {
    ratio_clean(&tmp);
    return NULL;
  }
  ratio_clean(&tmp);
  return dest;
}

s_ratio * ratio_init (s_ratio *dest)
{
  s_ratio tmp = {0};
  assert(dest);
  *dest = tmp;
  return dest;
}

s_ratio * ratio_init_1 (s_ratio *q, const char *p)
{
  s_buf buf = {0};
  uw len;
  sw r;
  s_ratio tmp = {0};
  assert(q);
  assert(p);
  if (! p)
    return NULL;
  len = strlen(p);
  buf_init_const(&buf, len, p); // buf is read-only
  buf.wpos = len;
  r = buf_parse_ratio(&buf, &tmp);
  if (r < 0 || (uw) r != len) {
    err_write_1("ratio_init_1: invalid ratio: \"");
    err_write_1(p);
    err_write_1("\", ");
    err_inspect_uw(len);
    err_write_1(" != ");
    err_inspect_sw(r);
    assert(! "ratio_init_1: invalid ratio");
    return NULL;
  }
  *q = tmp;
  return q;
}

s_ratio * ratio_init_cast (s_ratio *ratio, p_sym *type,
			   s_tag *src)
{
  assert(ratio);
  assert(type);
  assert(src);
  (void) type;
  switch (src->type) {
  case TAG_F32:
    return ratio_init_f32(ratio, src->data.f32);
  case TAG_F64:
    return ratio_init_f64(ratio, src->data.f64);
#if HAVE_F80
  case TAG_F80:
    return ratio_init_f80(ratio, src->data.f80);
#endif
#if HAVE_F128
  case TAG_F128:
    return ratio_init_f128(ratio, src->data.f128);
#endif
  case TAG_INTEGER:
    return ratio_init_integer(ratio, &src->data.integer);
  case TAG_RATIO:
    return ratio_init_copy(ratio, &src->data.ratio);
  case TAG_SW:
    return ratio_init_sw(ratio, src->data.sw);
  case TAG_S64:
    return ratio_init_s64(ratio, src->data.s64);
  case TAG_S32:
    return ratio_init_s32(ratio, src->data.s32);
  case TAG_S16:
    return ratio_init_s16(ratio, src->data.s16);
  case TAG_S8:
    return ratio_init_s8(ratio, src->data.s8);
  case TAG_U8:
    return ratio_init_u8(ratio, src->data.u8);
  case TAG_U16:
    return ratio_init_u16(ratio, src->data.u16);
  case TAG_U32:
    return ratio_init_u32(ratio, src->data.u32);
  case TAG_U64:
    return ratio_init_u64(ratio, src->data.u64);
  case TAG_UW:
    return ratio_init_uw(ratio, src->data.uw);
  default:
    break;
  }
  err_puts("ratio_init_cast: invalid tag type");
  assert(! "ratio_init_cast: invalid tag type");
  return NULL;
}

s_ratio * ratio_init_copy (s_ratio *dest, const s_ratio *src)
{
  s_ratio tmp = {0};
  assert(src);
  assert(dest);
  if (! integer_init_copy(&tmp.numerator, &src->numerator))
    return NULL;
  if (! integer_init_copy(&tmp.denominator, &src->denominator)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  *dest = tmp;
  return dest;
}

s_ratio * ratio_init_f32 (s_ratio *r, f32 x)
{
  assert(r);
  ratio_init(r);
  integer_init_f32(&r->numerator, x);
  integer_init_s32(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_f64 (s_ratio *r, f64 x)
{
  assert(r);
  ratio_init(r);
  integer_init_f64(&r->numerator, x);
  integer_init_s32(&r->denominator, 1);
  return r;
}

#if HAVE_F80

s_ratio * ratio_init_f80 (s_ratio *r, f80 x)
{
  assert(r);
  ratio_init(r);
  integer_init_f80(&r->numerator, x);
  integer_init_s32(&r->denominator, 1);
  return r;
}

#endif

#if HAVE_F128

s_ratio * ratio_init_f128 (s_ratio *r, f128 x)
{
  assert(r);
  ratio_init(r);
  integer_init_f128(&r->numerator, x);
  integer_init_s32(&r->denominator, 1);
  return r;
}

#endif

s_ratio * ratio_init_integer (s_ratio *r, const s_integer *src)
{
  s_ratio tmp = {0};
  assert(r);
  assert(src);
  if (! integer_init_copy(&tmp.numerator, src))
    return NULL;
  if (! integer_init_u8(&tmp.denominator, 1)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  *r = tmp;
  return r;
}

s_ratio * ratio_init_s8(s_ratio *r, s8 x)
{
  assert(r);
  ratio_init(r);
  integer_init_s8(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_s16(s_ratio *r, s16 x)
{
  assert(r);
  ratio_init(r);
  integer_init_s16(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_s32 (s_ratio *r, s32 x)
{
  assert(r);
  ratio_init(r);
  integer_init_s32(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_s64 (s_ratio *r, s64 x)
{
  assert(r);
  ratio_init(r);
  integer_init_s64(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_sw (s_ratio *r, sw x)
{
  assert(r);
  ratio_init(r);
  integer_init_sw(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_u8 (s_ratio *r, u8 x)
{
  assert(r);
  ratio_init(r);
  integer_init_u8(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_u16 (s_ratio *r, u16 x)
{
  assert(r);
  ratio_init(r);
  integer_init_u16(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_u32 (s_ratio *r, u32 x)
{
  assert(r);
  ratio_init(r);
  integer_init_u32(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_u64 (s_ratio *r, u64 x)
{
  assert(r);
  ratio_init(r);
  integer_init_u64(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_uw (s_ratio *r, uw x)
{
  assert(r);
  ratio_init(r);
  integer_init_uw(&r->numerator, x);
  integer_init_u8(&r->denominator, 1);
  return r;
}

s_ratio * ratio_init_zero (s_ratio *r)
{
  s_ratio tmp = {0};
  assert(r);
  if (! integer_init_zero(&tmp.numerator))
    return NULL;
  if (! integer_init_u8(&tmp.denominator, 1)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  *r = tmp;
  return r;
}

bool ratio_is_negative (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));
  return integer_is_negative(&r->numerator);
}

bool ratio_is_zero (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));
  return integer_is_zero(&r->numerator);
}

s_ratio * ratio_mul (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest)
{
  s_ratio tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  assert(integer_is_positive(&a->denominator));
  assert(integer_is_positive(&b->denominator));
  if (! integer_mul(&a->numerator, &b->numerator, &tmp.numerator))
    return NULL;
  if (! integer_mul(&a->denominator, &b->denominator,
                    &tmp.denominator)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  if (! ratio_simplify(&tmp, dest)) {
    ratio_clean(&tmp);
    return NULL;
  }
  ratio_clean(&tmp);
  return dest;
}

s_ratio * ratio_neg (const s_ratio *r, s_ratio *dest)
{
  s_ratio tmp = {0};
  assert(r);
  assert(dest);
  assert(integer_is_positive(&r->denominator));
  if (! integer_neg(&r->numerator, &tmp.numerator))
    return NULL;
  if (! integer_init_copy(&tmp.denominator, &r->denominator)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  *dest = tmp;
  return dest;
}

s_ratio * ratio_pow (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest)
{
  static bool      static_init;
  static s_integer static_one = {0};
  static s_integer static_u32_max;
  s_ratio tmp = {0};
  u32 n;
  assert(a);
  assert(b);
  assert(dest);
  assert(integer_is_positive(&a->denominator));
  assert(integer_is_positive(&b->denominator));
  if (! static_init) {
    integer_init_u8(&static_one, 1);
    integer_init_u32(&static_u32_max, U32_MAX);
    static_init = true;
  }
  if (compare_integer(&static_u32_max, &b->numerator) < 0) {
    err_puts("ratio_pow: b > U32_MAX");
    assert(! "ratio_pow: b > U32_MAX");
    return NULL;
  }
  n = integer_to_u32(&b->numerator);
  integer_expt_u32(&a->numerator, n, &tmp.numerator);
  integer_expt_u32(&a->denominator, n, &tmp.denominator); 
  if (! ratio_simplify(&tmp, dest)) {
    ratio_clean(&tmp);
    return NULL;
  }
  ratio_clean(&tmp);
  return dest;
}

s_ratio * ratio_simplify (s_ratio *r, s_ratio *dest)
{
  s_ratio tmp;
  s_integer gcd;
  assert(r);
  assert(dest);
  assert(integer_is_positive(&r->denominator));
  if (! integer_gcd(&r->numerator, &r->denominator, &gcd))
    return NULL;
  if (! integer_div(&r->numerator, &gcd, &tmp.numerator)) {
    integer_clean(&gcd);
    return NULL;
  }
  if (! integer_div(&r->denominator, &gcd, &tmp.denominator)) {
    integer_clean(&tmp.numerator);
    integer_clean(&gcd);
    return NULL;
  }
  integer_clean(&gcd);
  *dest = tmp;
  return dest;
}

s_tag * ratio_sqrt (const s_ratio *r, s_tag *dest)
{
  s_ratio tmp = {0};
  assert(r);
  assert(dest);
  assert(integer_is_positive(&r->denominator));
  if (ratio_is_negative(r)) {
    // FIXME
    err_puts("ratio_sqrt: not implemented");
    assert(! "ratio_sqrt: not implemented");
    return NULL;
  }
  if (! integer_sqrt_positive(&r->numerator, &tmp.numerator))
    return NULL;
  if (! integer_sqrt_positive(&r->denominator, &tmp.denominator)) {
    integer_clean(&tmp.numerator);
    return NULL;
  }
  dest->type = TAG_RATIO;
  dest->data.ratio = tmp;
  return dest;
}

s_ratio * ratio_sub (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest)
{
  s_ratio tmp = {0};
  s_integer i = {0};
  s_integer j = {0};
  assert(a);
  assert(b);
  assert(dest);
  assert(integer_is_positive(&a->denominator));
  assert(integer_is_positive(&b->denominator));
  if (! integer_mul(&a->numerator, &b->denominator, &i)) {
    ratio_clean(&tmp);
    return NULL;
  }
  if (! integer_mul(&b->numerator, &a->denominator, &j)) {
    integer_clean(&i);
    ratio_clean(&tmp);
    return NULL;
  }
  if (! integer_sub(&i, &j, &tmp.numerator)) {
    integer_clean(&i);
    integer_clean(&j);
    ratio_clean(&tmp);
    return NULL;
  }
  integer_clean(&i);
  integer_clean(&j);
  if (! integer_mul(&a->denominator, &b->denominator,
                    &tmp.denominator)) {
    ratio_clean(&tmp);
    return NULL;
  }
  if (! ratio_simplify(&tmp, dest)) {
    ratio_clean(&tmp);
    return NULL;
  }
  ratio_clean(&tmp);
  return dest;
}

f32 ratio_to_f32 (const s_ratio *r)
{
  f32 numerator;
  f32 denominator;
  assert(r);
  assert(integer_is_positive(&r->denominator));
  numerator = integer_to_f32(&r->numerator);
  denominator = integer_to_f32(&r->denominator);
  return numerator / denominator;
}

f64 ratio_to_f64 (const s_ratio *r)
{
  f64 numerator;
  f64 denominator;
  assert(r);
  assert(integer_is_positive(&r->denominator));
  numerator = integer_to_f64(&r->numerator);
  denominator = integer_to_f64(&r->denominator);
  return numerator / denominator;
}

#if HAVE_F80

f80 ratio_to_f80 (const s_ratio *r)
{
  f80 numerator;
  f80 denominator;
  assert(r);
  assert(integer_is_positive(&r->denominator));
  numerator = integer_to_f80(&r->numerator);
  denominator = integer_to_f80(&r->denominator);
  return numerator / denominator;
}

#endif

#if HAVE_F128

f128 ratio_to_f128 (const s_ratio *r)
{
  f128 numerator;
  f128 denominator;
  assert(r);
  assert(integer_is_positive(&r->denominator));
  numerator = integer_to_f128(&r->numerator);
  denominator = integer_to_f128(&r->denominator);
  return numerator / denominator;
}

#endif

s8 ratio_to_s8 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  s8 numerator = integer_to_s8(&r->numerator);
  s8 denominator = integer_to_s8(&r->denominator);

  return numerator / denominator;
}

s16 ratio_to_s16 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  s16 numerator = integer_to_s16(&r->numerator);
  s16 denominator = integer_to_s16(&r->denominator);

  return numerator / denominator;
}

s32 ratio_to_s32 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  s32 numerator = integer_to_s32(&r->numerator);
  s32 denominator = integer_to_s32(&r->denominator);

  return numerator / denominator;
}

s64 ratio_to_s64 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  s64 numerator = integer_to_s64(&r->numerator);
  s64 denominator = integer_to_s64(&r->denominator);

  return numerator / denominator;
}

sw ratio_to_sw (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  sw numerator = integer_to_sw(&r->numerator);
  sw denominator = integer_to_sw(&r->denominator);

  return numerator / denominator;
}

u8 ratio_to_u8 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  u8 numerator = integer_to_u8(&r->numerator);
  u8 denominator = integer_to_u8(&r->denominator);

  return numerator / denominator;
}

u16 ratio_to_u16 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  u16 numerator = integer_to_u16(&r->numerator);
  u16 denominator = integer_to_u16(&r->denominator);

  return numerator / denominator;
}

u32 ratio_to_u32 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  u32 numerator = integer_to_u32(&r->numerator);
  u32 denominator = integer_to_u32(&r->denominator);

  return numerator / denominator;
}

u64 ratio_to_u64 (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  u64 numerator = integer_to_u64(&r->numerator);
  u64 denominator = integer_to_u64(&r->denominator);

  return numerator / denominator;
}

uw ratio_to_uw (const s_ratio *r)
{
  assert(r);
  assert(integer_is_positive(&r->denominator));

  uw numerator = integer_to_uw(&r->numerator);
  uw denominator = integer_to_uw(&r->denominator);

  return numerator / denominator;
}
