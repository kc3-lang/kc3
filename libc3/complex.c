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
#include <math.h>
#include "alloc.h"
#include "assert.h"
#include "complex.h"
#include "f32.h"
#include "f64.h"
#include "f128.h"
#include "sym.h"
#include "tag.h"
#include "tag_init.h"

#define DEF_COMPLEX_INIT(type)                                         \
  s_complex * complex_init_ ## type (s_complex *c, type src)           \
  {                                                                    \
    assert(c);                                                         \
    tag_init_ ## type(&c->x, src);                                     \
    tag_init_u8(&c->y, 0);                                             \
    return c;                                                          \
  }

DEF_COMPLEX_INIT(f32)
DEF_COMPLEX_INIT(f64)
DEF_COMPLEX_INIT(f128)
DEF_COMPLEX_INIT(s8)
DEF_COMPLEX_INIT(s16)
DEF_COMPLEX_INIT(s32)
DEF_COMPLEX_INIT(s64)
DEF_COMPLEX_INIT(sw)
DEF_COMPLEX_INIT(u8)
DEF_COMPLEX_INIT(u16)
DEF_COMPLEX_INIT(u32)
DEF_COMPLEX_INIT(u64)
DEF_COMPLEX_INIT(uw)

s_complex * complex_add (const s_complex *a, const s_complex *b,
                         s_complex *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  tag_add(&a->x, &b->x, &dest->x);
  tag_add(&a->y, &b->y, &dest->y);
  return dest;
}

void complex_clean (s_complex *c)
{
  assert(c);
  tag_clean(&c->x);
  tag_clean(&c->y);
}

void complex_delete (s_complex *c)
{
  assert(c);
  complex_clean(c);
  free(c);
}

s_complex * complex_div (const s_complex *a, const s_complex *b,
                         s_complex *dest)
{
  s_tag axbx;
  s_tag axby;
  s_tag aybx;
  s_tag ayby;
  s_tag axbx_ayby;
  s_tag aybx_axby;
  s_tag bx2;
  s_tag by2;
  s_tag bx2_by2;
  s_complex tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  assert(tag_is_number(&a->x));
  assert(tag_is_number(&a->y));
  assert(tag_is_number(&b->x));
  assert(tag_is_number(&b->y));
  //  a.x * b.x + a.y * b.y      a.y * b.x - a.x * b.y
  //  ---------------------  +i  ---------------------
  //  b.x * b.x + b.y * b.y      b.x * b.x + b.y * b.y
  tag_mul(&a->x, &b->x, &axbx);
  tag_mul(&a->y, &b->y, &ayby);
  tag_add(&axbx, &ayby, &axbx_ayby);
  tag_mul(&b->x, &b->x, &bx2);
  tag_mul(&b->y, &b->y, &by2);
  tag_add(&bx2, &by2, &bx2_by2);
  tag_div(&axbx_ayby, &bx2_by2, &tmp.x);
  tag_mul(&a->y, &b->x, &aybx);
  tag_mul(&a->x, &b->y, &axby);
  tag_sub(&aybx, &axby, &aybx_axby);
  tag_div(&aybx_axby, &bx2_by2, &tmp.y);
  *dest = tmp;
  return dest;
}

s_complex * complex_init (s_complex *c)
{
  assert(c);
  tag_init_u8(&c->x, 0);
  tag_init_u8(&c->y, 0);
  return c;
}

s_complex * complex_init_cast (s_complex *c, const s_tag *src)
{
  assert(c);
  assert(src);
  switch (src->type) {
  case TAG_COMPLEX:
    return complex_init_copy(c, src->data.complex);
  case TAG_F32:
    return complex_init_f32(c, src->data.f32);
  case TAG_F64:
    return complex_init_f64(c, src->data.f64);
  case TAG_F128:
    return complex_init_f128(c, src->data.f128);
  case TAG_INTEGER:
    return complex_init_integer(c, &src->data.integer);
  case TAG_RATIO:
    return complex_init_ratio(c, &src->data.ratio);
  case TAG_SW:
    return complex_init_sw(c, src->data.sw);
  case TAG_S64:
    return complex_init_s64(c, src->data.s64);
  case TAG_S32:
    return complex_init_s32(c, src->data.s32);
  case TAG_S16:
    return complex_init_s16(c, src->data.s16);
  case TAG_S8:
    return complex_init_s8(c, src->data.s8);
  case TAG_U8:
    return complex_init_u8(c, src->data.u8);
  case TAG_U16:
    return complex_init_u16(c, src->data.u16);
  case TAG_U32:
    return complex_init_u32(c, src->data.u32);
  case TAG_U64:
    return complex_init_u64(c, src->data.u64);
  case TAG_UW:
    return complex_init_uw(c, src->data.uw);
  default:
    break;
  }
  err_puts("complex_init_cast: invalid tag type");
  assert(! "complex_init_cast: invalid tag type");
  return NULL;
}

s_complex * complex_init_copy (s_complex *c, const s_complex *src)
{
  assert(c);
  assert(src);
  tag_init_copy(&c->x, &src->x);
  tag_init_copy(&c->y, &src->y);
  return c;
}

s_complex * complex_init_integer (s_complex *c, const s_integer *src)
{
  assert(c);
  tag_init_integer_copy(&c->x, src);
  tag_init_u8(&c->y, 0);
  return c;
}

s_complex * complex_init_ratio (s_complex *c, const s_ratio *src)
{
  assert(c);
  tag_init_ratio_copy(&c->x, src);
  tag_init_u8(&c->y, 0);
  return c;
}

s_complex * complex_mul (const s_complex *a, const s_complex *b,
                         s_complex *dest)
{
  s_tag axbx;
  s_tag axby;
  s_tag aybx;
  s_tag ayby;
  assert(a);
  assert(b);
  assert(dest);
  tag_mul(&a->x, &b->x, &axbx);
  tag_mul(&a->y, &b->y, &ayby);
  tag_mul(&a->x, &b->y, &axby);
  tag_mul(&a->y, &b->x, &aybx);
  tag_sub(&axbx, &ayby, &dest->x);
  tag_add(&axby, &aybx, &dest->y);
  tag_clean(&axbx);
  tag_clean(&axby);
  tag_clean(&aybx);
  tag_clean(&ayby);
  return dest;
}

s_complex * complex_new (void)
{
  s_complex *c;
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_init(c)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_new_add (const s_complex *a, const s_complex *b)
{
  s_complex *c;
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_add(a, b, c)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_new_cast (const s_tag *src)
{
  s_complex *c;
  assert(src);
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_init_cast(c, src)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_new_div (const s_complex *a, const s_complex *b)
{
  s_complex *c;
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_div(a, b, c)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_new_mul (const s_complex *a, const s_complex *b)
{
  s_complex *c;
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_mul(a, b, c)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_new_sub (const s_complex *a, const s_complex *b)
{
  s_complex *c;
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_sub(a, b, c)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_new_copy (const s_complex *src)
{
  s_complex *c;
  assert(src);
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_init_copy(c, src)) {
    free(c);
    return NULL;
  }
  return c;
}

s_complex * complex_sub (const s_complex *a, const s_complex *b,
                         s_complex *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  tag_sub(&a->x, &b->x, &dest->x);
  tag_sub(&a->y, &b->y, &dest->y);
  return dest;
}

s_tag * complex_norm (const s_complex *c, s_tag *dest)
{
  s_complex d;
  s_tag sum;
  s_tag tmp;
  assert(c);
  tag_mul(&c->x, &c->x, &d.x);
  tag_mul(&c->y, &c->y, &d.y);
  tag_add(&d.x, &d.y, &sum);
  tag_sqrt(&sum, &tmp);
  tag_clean(&sum);
  complex_clean(&d);
  *dest = tmp;
  return dest;
}

bool complex_is_zero(const s_complex *c)
{
  assert(c);
  return tag_is_zero(&c->x) && tag_is_zero(&c->y);
}

f32 complex_to_f32 (const s_complex *c)
{
  s_tag norm;
  f32 x;
  assert(c);
  complex_norm(c, &norm);
  f32_init_cast(&x, &g_sym_F32, &norm);
  return x;
}

f64 complex_to_f64 (const s_complex *c)
{
  s_tag norm;
  f64 x;
  assert(c);
  complex_norm(c, &norm);
  f64_init_cast(&x, &g_sym_F64, &norm);
  return x;
}

f128 complex_to_f128 (const s_complex *c)
{
  s_tag norm;
  f128 x;
  assert(c);
  complex_norm(c, &norm);
  f128_init_cast(&x, &g_sym_F128, &norm);
  return x;
}
