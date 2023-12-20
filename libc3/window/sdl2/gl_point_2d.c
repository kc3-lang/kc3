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
#include "gl_point_2d.h"

s_gl_point_2d * gl_point_2d_init (s_gl_point_2d *p, f64 x, f64 y)
{
  assert(p);
  p->x = x;
  p->y = y;
  return p;
}

s_gl_point_2d * gl_point_2d_init_copy (s_gl_point_2d *p,
                                       const s_gl_point_2d *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  return p;
}

s_gl_point_2d *
gl_point_2d_init_product (s_gl_point_2d *p,
                          const s_gl_matrix_3d *m,
                          const s_gl_point_2d *s)
{
  assert(p);
  assert(m);
  assert(s);
  p->x = s->x * m->xx + s->y * m->xy + m->xz;
  p->y = s->x * m->yx + s->y * m->yy + m->yz;
  return p;
}

s_gl_point_2d * gl_point_2d_init_zero (s_gl_point_2d *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  return p;
}

void gl_point_2d_delete (s_gl_point_2d *p)
{
  free(p);
}

s_gl_point_2d * gl_point_2d_new (f64 x, f64 y)
{
  s_gl_point_2d *p;
  p = calloc(1, sizeof(s_gl_point_2d));
  if (! p) {
    err_puts("gl_point_2d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_2d_init(p, x, y);
  return p;
}

s_gl_point_2d * gl_point_2d_new_copy (const s_gl_point_2d *src)
{
  s_gl_point_2d *p;
  p = calloc(1, sizeof(s_gl_point_2d));
  if (! p) {
    err_puts("gl_point_2d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_2d_init_copy(p, src);
  return p;
}

s_gl_point_2d * gl_point_2d_new_zero (void)
{
  s_gl_point_2d *p;
  p = calloc(1, sizeof(s_gl_point_2d));
  if (! p) {
    err_puts("gl_point_2d_new: failed to allocate memory");
    return NULL;
  }
  gl_point_2d_init_zero(p);
  return p;
}
