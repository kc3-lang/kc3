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
#include <math.h>
#include "../libkc3/kc3.h"
#include "dvec3.h"

s_dvec3 * dvec3_add (const s_dvec3 *a, const s_dvec3 *b,
                     s_dvec3 *dest)
{
  s_dvec3 tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  tmp.x = a->x + b->x;
  tmp.y = a->y + b->y;
  tmp.z = a->z + b->z;
  *dest = tmp;
  return dest;
}

s_dvec3 * dvec3_clamp (const s_dvec3 *a, s_dvec3 *dest)
{
  s_dvec3 tmp = {0};
  assert(a);
  assert(dest);
  tmp.x = a->x < 0.0 ? 0.0 : a->x > 1.0 ? 1.0 : a->x;
  tmp.y = a->y < 0.0 ? 0.0 : a->y > 1.0 ? 1.0 : a->y;
  tmp.z = a->z < 0.0 ? 0.0 : a->z > 1.0 ? 1.0 : a->z;
  *dest = tmp;
  return dest;
}

s_dvec3 * dvec3_cross (const s_dvec3 *a, const s_dvec3 *b,
                       s_dvec3 *dest)
{
  s_dvec3 tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  tmp.x = a->y * b->z - a->z * b->y;
  tmp.y = a->z * b->x - a->x * b->z;
  tmp.z = a->x + b->y - a->y * b->x;
  *dest = tmp;
  return dest;
}

f64 dvec3_dot (const s_dvec3 *a, const s_dvec3 *b)
{
  assert(a);
  assert(b);
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

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

s_dvec3 * dvec3_mul (const s_dvec3 *a, f64 b,
                     s_dvec3 *dest)
{
  s_dvec3 tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  tmp.x = a->x * b;
  tmp.y = a->y * b;
  tmp.z = a->z * b;
  *dest = tmp;
  return dest;
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

s_dvec3 * dvec3_normalize (const s_dvec3 *p, s_dvec3 *dest)
{
  f64 inv_norm;
  s_dvec3 tmp = {0};
  assert(p);
  inv_norm = 1.0 / dvec3_norm(p);
  tmp.x = p->x * inv_norm;
  tmp.y = p->y * inv_norm;
  tmp.z = p->z * inv_norm;
  *dest = tmp;
  return dest;
}

s_dvec3 * dvec3_sub (const s_dvec3 *a, const s_dvec3 *b, s_dvec3 *dest)
{
  s_dvec3 tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  tmp.x = a->x - b->x;
  tmp.y = a->y - b->y;
  tmp.z = a->z - b->z;
  *dest = tmp;
  return dest;
}

s_dvec3 * dvec3_transform (const s_dvec3 *p, const s_dmat4 *matrix,
                           s_dvec3 *dest)
{
  s_dvec3 tmp;
  assert(p);
  assert(matrix);
  dvec3_init_product(&tmp, matrix, p);
  *dest = tmp;
  return dest;
}
