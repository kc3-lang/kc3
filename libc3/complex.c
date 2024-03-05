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
#include "tag.h"
#include "tag_init.h"

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

s_complex * complex_init_copy (s_complex *c, const s_complex *src)
{
  assert(c);
  assert(src);
  tag_init_copy(&c->x, &src->x);
  tag_init_copy(&c->y, &src->y);
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
  f32_init_cast(&x, &norm);
  return x;
}

f64 complex_to_f64 (const s_complex *c)
{
  s_tag norm;
  f64 x;
  assert(c);
  complex_norm(c, &norm);
  f64_init_cast(&x, &norm);
  return x;
}
