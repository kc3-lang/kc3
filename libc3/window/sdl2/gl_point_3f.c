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
#include "gl_point_3f.h"

s_gl_point_3f * gl_point_3f_init (s_gl_point_3f *p, f32 x, f32 y, f32 z)
{
  assert(p);
  p->x = x;
  p->y = y;
  p->z = z;
  return p;
}

s_gl_point_3f * gl_point_3f_init_copy (s_gl_point_3f *p,
                                       const s_gl_point_3f *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  p->z = src->z;
  return p;
}

s_gl_point_3f * gl_point_3f_init_normalize (s_gl_point_3f *p,
                                            const s_gl_point_3f *src)
{
  f32 norm;
  f32 ratio;
  assert(p);
  assert(src);
  norm = gl_point_3f_norm(src);
  ratio = 1.0f / norm;
  p->x = src->x * ratio;
  p->y = src->y * ratio;
  p->z = src->z * ratio;
  return p;
}

s_gl_point_3f * gl_point_3f_init_product (s_gl_point_3f *p,
                                          const s_gl_matrix_4f *m,
                                          const s_gl_point_3f *s)
{
  assert(p);
  assert(m);
  assert(s);
  p->x = m->xx * s->x + m->xy * s->y + m->xz * s->z + m->xt;
  p->y = m->xy * s->x + m->yy * s->y + m->yz * s->z + m->yt;
  p->z = m->xz * s->x + m->zy * s->y + m->zz * s->z + m->zt;
  return p;
}

s_gl_point_3f * gl_point_3f_init_zero (s_gl_point_3f *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  p->z = 0.0;
  return p;
}

void gl_point_3f_delete (s_gl_point_3f *p)
{
  free(p);
}

s_gl_point_3f * gl_point_3f_new (f32 x, f32 y, f32 z)
{
  s_gl_point_3f *p;
  p = calloc(1, sizeof(s_gl_point_3f));
  if (! p) {
    err_puts("gl_point_3f_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3f_init(p, x, y, z);
  return p;
}

s_gl_point_3f * gl_point_3f_new_copy (const s_gl_point_3f *src)
{
  s_gl_point_3f *p;
  p = calloc(1, sizeof(s_gl_point_3f));
  if (! p) {
    err_puts("gl_point_3f_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3f_init_copy(p, src);
  return p;
}

s_gl_point_3f * gl_point_3f_new_product (const s_gl_matrix_4f *m,
                                         const s_gl_point_3f *s)
{
  s_gl_point_3f *p;
  assert(m);
  assert(s);
  p = calloc(1, sizeof(s_gl_point_3f));
  if (! p) {
    err_puts("gl_point_3f_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3f_init_product(p, m, s);
  return p;
}

s_gl_point_3f * gl_point_3f_new_zero (void)
{
  s_gl_point_3f *p;
  p = calloc(1, sizeof(s_gl_point_3f));
  if (! p) {
    err_puts("gl_point_3f_new: failed to allocate memory");
    return NULL;
  }
  gl_point_3f_init_zero(p);
  return p;
}

f32 gl_point_3f_norm (const s_gl_point_3f *p)
{
  assert(p);
  return sqrtf(p->x * p->x + p->y * p->y + p->z * p->z);
}

void gl_point_3f_normalize (s_gl_point_3f *p)
{
  f32 norm;
  f32 ratio;
  assert(p);
  norm = gl_point_3f_norm(p);
  ratio = 1.0f / norm;
  p->x *= ratio;
  p->y *= ratio;
  p->z *= ratio;
}

void gl_point_3f_transform (s_gl_point_3f *p,
                            const s_gl_matrix_4f *matrix)
{
  s_gl_point_3f tmp;
  assert(p);
  assert(matrix);
  gl_point_3f_init_product(&tmp, matrix, p);
  *p = tmp;
}
