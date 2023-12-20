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
#include <math.h>
#include <libc3/c3.h>
#include "gl_point_3d.h"

s_gl_point_3d * gl_point_3d_init (s_gl_point_3d *p, f64 x, f64 y, f64 z)
{
  assert(p);
  p->x = x;
  p->y = y;
  p->z = z;
  return p;
}

s_gl_point_3d * gl_point_3d_init_copy (s_gl_point_3d *p,
                                       const s_gl_point_3d *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  p->z = src->z;
  return p;
}

s_gl_point_3d * gl_point_3d_init_normalize (s_gl_point_3d *p,
                                            const s_gl_point_3d *src)
{
  f64 r;
  assert(p);
  assert(src);
  r = 1.0 / gl_point_3d_norm(src);
  p->x = src->x * r;
  p->y = src->y * r;
  p->z = src->z * r;
  return p;
}

s_gl_point_3d * gl_point_3d_init_product (s_gl_point_3d *p,
                                          const s_gl_matrix_4d *m,
                                          const s_gl_point_3d *s)
{
  assert(p);
  assert(m);
  assert(s);
  p->x = s->x * m->xx + s->y * m->xy + s->z * m->xz + m->xt;
  p->y = s->x * m->yx + s->y * m->yy + s->z * m->yz + m->yt;
  p->z = s->x * m->zx + s->y * m->zy + s->z * m->zz + m->zt;
  return p;
}

s_gl_point_3d * gl_point_3d_init_zero (s_gl_point_3d *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  p->z = 0.0;
  return p;
}

void gl_point_3d_delete (s_gl_point_3d *p)
{
  free(p);
}

s_gl_point_3d * gl_point_3d_new (f64 x, f64 y, f64 z)
{
  s_gl_point_3d *p;
  p = calloc(1, sizeof(s_gl_point_3d));
  if (! p) {
    err_puts("gl_point_3d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3d_init(p, x, y, z);
  return p;
}

s_gl_point_3d * gl_point_3d_new_copy (const s_gl_point_3d *src)
{
  s_gl_point_3d *p;
  p = calloc(1, sizeof(s_gl_point_3d));
  if (! p) {
    err_puts("gl_point_3d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3d_init_copy(p, src);
  return p;
}

s_gl_point_3d * gl_point_3d_new_product (const s_gl_matrix_4d *m,
                                         const s_gl_point_3d *s)
{
  s_gl_point_3d *p;
  assert(m);
  assert(s);
  p = calloc(1, sizeof(s_gl_point_3d));
  if (! p) {
    err_puts("gl_point_3d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3d_init_product(p, m, s);
  return p;
}

s_gl_point_3d * gl_point_3d_new_zero (void)
{
  s_gl_point_3d *p;
  p = calloc(1, sizeof(s_gl_point_3d));
  if (! p) {
    err_puts("gl_point_3d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3d_init_zero(p);
  return p;
}

f64 gl_point_3d_norm (const s_gl_point_3d *p)
{
  assert(p);
  return sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
}
