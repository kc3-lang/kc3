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
#include <stdlib.h>
#include "tag.h"
#include "tag_init.h"
#include <math.h>

s_complex * complex_init (s_complex *c)
{
  assert(c);
  tag_init_f64(&c->x, 0);
  tag_init_f64(&c->y, 0);
  return c;
}

s_complex * complex_init_copy (s_complex *a, const s_complex *x)
{
  assert(a);
  assert(x);
  tag_init_copy(&a->x, &x->x);
  tag_init_copy(&a->y, &x->y);
  return a;
}

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

s_complex * complex_mul (const s_complex *a, const s_complex *b,
                         s_complex *dest)
{
  assert(a);
  assert(b);
  assert(dest);

  s_tag ac, bd, ad, bc, ac_bd, ad_bc;

  // ac = a.x * b.x
  tag_mul(&a->x, &b->x, &ac);
  // bd = a.y * b.y
  tag_mul(&a->y, &b->y, &bd);
  // ad = a.x * b.y
  tag_mul(&a->x, &b->y, &ad);
  // bc = a.y * b.x
  tag_mul(&a->y, &b->x, &bc);

  // ac_bd = ac - bd
  tag_sub(&ac, &bd, &ac_bd);
  // ad_bc = ad + bc
  tag_add(&ad, &bc, &ad_bc);

  // dest.x = ac_bd
  tag_init_copy(&dest->x, &ac_bd);
  // dest.y = ad_bc
  tag_init_copy(&dest->y, &ad_bc);

  return dest;
}

s_complex * complex_new (void)
{
  s_complex *c = malloc(sizeof(s_complex));
  complex_init(c);
  return c;
}

s_complex * complex_new_copy (const s_complex *a)
{
  s_complex *c = malloc(sizeof(s_complex));
  complex_init_copy(c, a);
  return c;
}

bool complex_is_zero(const s_complex *c)
{
  assert(c);
  return tag_is_zero(&c->x) && tag_is_zero(&c->y);
}

f32 complex_to_f32 (const s_complex *c)
{
  assert(c);
  f32 real = tag_to_f32(&c->x);
  f32 imag = tag_to_f32(&c->y);
  return sqrtf(real * real + imag * imag);
}

f64 complex_to_f64 (const s_complex *c)
{
  assert(c);
  f64 real = tag_to_f64(&c->x);
  f64 imag = tag_to_f64(&c->y);
  return sqrt(real * real + imag * imag);
}