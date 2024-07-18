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
#include <libc3/c3.h>
#include "dvec3.h"

s_dvec3 * dvec3_init (s_dvec3 *p, f64 x, f64 y, f64 z)
{
  assert(p);
  p->x = x;
  p->y = y;
  p->z = z;
  return p;
}

s_dvec3 * dvec3_init_copy (s_dvec3 *p, const s_dvec3 *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  p->z = src->z;
  return p;
}

s_dvec3 * dvec3_init_normalize (s_dvec3 *p, const s_dvec3 *src)
{
  f64 r;
  assert(p);
  assert(src);
  r = 1.0 / dvec3_norm(src);
  p->x = src->x * r;
  p->y = src->y * r;
  p->z = src->z * r;
  return p;
}

s_dvec3 * dvec3_init_product (s_dvec3 *p, const s_dmat4 *m,
                              const s_dvec3 *s)
{
  assert(p);
  assert(m);
  assert(s);
  p->x = s->x * m->xx + s->y * m->xy + s->z * m->xz + m->xt;
  p->y = s->x * m->yx + s->y * m->yy + s->z * m->yz + m->yt;
  p->z = s->x * m->zx + s->y * m->zy + s->z * m->zz + m->zt;
  return p;
}

s_dvec3 * dvec3_init_zero (s_dvec3 *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  p->z = 0.0;
  return p;
}

void dvec3_delete (s_dvec3 *p)
{
  free(p);
}

s_dvec3 * dvec3_new (f64 x, f64 y, f64 z)
{
  s_dvec3 *p;
  p = calloc(1, sizeof(s_dvec3));
  if (! p) {
    err_puts("dvec3_new: failed to allocate memory");
    return NULL;
  }
  dvec3_init(p, x, y, z);
  return p;
}

s_dvec3 * dvec3_new_copy (const s_dvec3 *src)
{
  s_dvec3 *p;
  p = calloc(1, sizeof(s_dvec3));
  if (! p) {
    err_puts("dvec3_new: failed to allocate memory");
    return NULL;
  }
  dvec3_init_copy(p, src);
  return p;
}

s_dvec3 * dvec3_new_product (const s_dmat4 *m, const s_dvec3 *s)
{
  s_dvec3 *p;
  assert(m);
  assert(s);
  p = calloc(1, sizeof(s_dvec3));
  if (! p) {
    err_puts("dvec3_new: failed to allocate memory");
    return NULL;
  }
  dvec3_init_product(p, m, s);
  return p;
}

s_dvec3 * dvec3_new_zero (void)
{
  s_dvec3 *p;
  p = calloc(1, sizeof(s_dvec3));
  if (! p) {
    err_puts("dvec3_new: failed to allocate memory");
    return NULL;
  }
  dvec3_init_zero(p);
  return p;
}

f64 dvec3_norm (const s_dvec3 *p)
{
  assert(p);
  return sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
}

void dvec3_normalize (s_dvec3 *p)
{
  f64 inv_norm;
  assert(p);
  inv_norm = 1.0 / dvec3_norm(p);
  p->x *= inv_norm;
  p->y *= inv_norm;
  p->z *= inv_norm;
}

void dvec3_transform (s_dvec3 *p, const s_dmat4 *matrix)
{
  s_dvec3 tmp;
  assert(p);
  assert(matrix);
  dvec3_init_product(&tmp, matrix, p);
  *p = tmp;
}
