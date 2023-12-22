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
#include "gl_matrix_4d.h"
#include "gl_point_3d.h"

s_gl_matrix_4d * gl_matrix_4d_init_copy (s_gl_matrix_4d *m,
                                         const s_gl_matrix_4d *src)
{
  assert(m);
  assert(src);
  *m = *src;
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_init_product (s_gl_matrix_4d *m,
                                            const s_gl_matrix_4d *a,
                                            const s_gl_matrix_4d *b)
{
  assert(m);
  assert(a);
  assert(b);
  m->xx = a->xx * b->xx + a->xy * b->yx + a->xz * b->zx + a->xt * b->tx;
  m->xy = a->xx * b->xy + a->xy * b->yy + a->xz * b->zy + a->xt * b->ty;
  m->xz = a->xx * b->xz + a->xy * b->yz + a->xz * b->zz + a->xt * b->tz;
  m->xt = a->xx * b->xt + a->xy * b->yt + a->xz * b->zt + a->xt * b->tt;
  m->yx = a->yx * b->xx + a->yy * b->yx + a->yz * b->zx + a->yt * b->tx;
  m->yy = a->yx * b->xy + a->yy * b->yy + a->yz * b->zy + a->yt * b->ty;
  m->yz = a->yx * b->xz + a->yy * b->yz + a->yz * b->zz + a->yt * b->tz;
  m->yt = a->yx * b->xt + a->yy * b->yt + a->yz * b->zt + a->yt * b->tt;
  m->zx = a->zx * b->xx + a->zy * b->yx + a->zz * b->zx + a->zt * b->tx;
  m->zy = a->zx * b->xy + a->zy * b->yy + a->zz * b->zy + a->zt * b->ty;
  m->zz = a->zx * b->xz + a->zy * b->yz + a->zz * b->zz + a->zt * b->tz;
  m->zt = a->zx * b->xt + a->zy * b->yt + a->zz * b->zt + a->zt * b->tt;
  m->tx = a->tx * b->xx + a->ty * b->yx + a->tz * b->zx + a->tt * b->tx;
  m->ty = a->tx * b->xy + a->ty * b->yy + a->tz * b->zy + a->tt * b->ty;
  m->tz = a->tx * b->xz + a->ty * b->yz + a->tz * b->zz + a->tt * b->tz;
  m->tt = a->tx * b->xt + a->ty * b->yt + a->tz * b->zt + a->tt * b->tt;
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_init_identity (s_gl_matrix_4d *m)
{
  assert(m);
  m->xx = 1.0; m->xy = 0.0; m->xz = 0.0; m->xt = 0.0;
  m->yx = 0.0; m->yy = 1.0; m->yz = 0.0; m->yt = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 1.0; m->zt = 0.0;
  m->tx = 0.0; m->ty = 0.0; m->tz = 0.0; m->tt = 1.0;
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_init_zero (s_gl_matrix_4d *m)
{
  assert(m);
  m->xx = 0.0; m->xy = 0.0; m->xz = 0.0; m->xt = 0.0;
  m->yx = 0.0; m->yy = 0.0; m->yz = 0.0; m->yt = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 0.0; m->zt = 0.0;
  m->tx = 0.0; m->ty = 0.0; m->tz = 0.0; m->tt = 0.0;
  return m;
}

void gl_matrix_4d_delete (s_gl_matrix_4d *m)
{
  free(m);
}

s_gl_matrix_4d * gl_matrix_4d_new_copy (const s_gl_matrix_4d *src)
{
  s_gl_matrix_4d *m;
  m = calloc(1, sizeof(s_gl_matrix_4d));
  if (! m) {
    err_puts("gl_matrix_4d_new: failed to allocate memory");
    return NULL;
  }
  *m = *src;
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_new_product (const s_gl_matrix_4d *a,
                                           const s_gl_matrix_4d *b)
{
  s_gl_matrix_4d *m;
  assert(a);
  assert(b);
  m = calloc(1, sizeof(s_gl_matrix_4d));
  if (! m) {
    err_puts("gl_matrix_4d_new: failed to allocate memory");
    return NULL;
  }
  gl_matrix_4d_init_product(m, a, b);
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_new_zero (void)
{
  s_gl_matrix_4d *m;
  m = calloc(1, sizeof(s_gl_matrix_4d));
  if (! m) {
    err_puts("gl_matrix_4d_new: failed to allocate memory");
    return NULL;
  }
  gl_matrix_4d_init_zero(m);
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_ortho (s_gl_matrix_4d *m, f64 x1, f64 x2,
                                     f64 y1, f64 y2, f64 clip_z_near,
                                     f64 clip_z_far)
{
  f64 dx;
  f64 dy;
  f64 dz;
  s_gl_matrix_4d ortho = {0};
  assert(m);
  dx = x2 - x1;
  dy = y2 - y1;
  dz = clip_z_far - clip_z_near;
  ortho.xx = 2.0 / dx;
  ortho.yy = 2.0 / dy;
  ortho.zz = -2.0 / dz;
  ortho.tx = -((x1 + x2) / dx);
  ortho.ty = -((y1 + y2) / dy);
  ortho.tz = -((clip_z_near + clip_z_far) / dz);
  gl_matrix_4d_product(m, &ortho);
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_perspective (s_gl_matrix_4d *m, f64 fov_y,
                                           f64 aspect_ratio,
                                           f64 z_near,
                                           f64 z_far)
{
  f64 dz;
  s_gl_matrix_4d perspective = {0};
  f64 f;
  f64 fov_y_2;
  fov_y_2 = fov_y / 2.0;
  f = cos(fov_y_2) / sin(fov_y_2);
  dz = z_near - z_far;
  perspective.xx = f / aspect_ratio;
  perspective.yy = f;
  perspective.zz = (z_near + z_far) / dz;
  perspective.zt = -1.0;
  perspective.tz = 2.0 * z_near * z_far / dz;
  gl_matrix_4d_product(m, &perspective);
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_product (s_gl_matrix_4d *m,
                                       const s_gl_matrix_4d *a)
{
  s_gl_matrix_4d tmp;
  gl_matrix_4d_init_product(&tmp, m, a);
  *m = tmp;
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_translate (s_gl_matrix_4d *m, f64 x,
                                         f64 y, f64 z)
{
  m->xt += x;
  m->yt += y;
  m->zt += z;
  return m;
}

s_gl_matrix_4d * gl_matrix_4d_rotate_axis (s_gl_matrix_4d *m, f64 rad,
                                           const s_gl_point_3d *axis)
{
  s_gl_point_3d a;
  f64 angle;
  f64 one_minus_x;
  f64 x;
  f64 y;
  gl_point_3d_init_normalize(&a, axis);
  angle = -rad;
  x = cos(angle);
  one_minus_x = 1.0 - x;
  y = sin(angle);
  s_gl_matrix_4d r = { x + a.x * a.x * one_minus_x,
                       a.x * a.y * one_minus_x - a.z * y,
                       a.x * a.z * one_minus_x + a.y * y,
                       0.0,
                       a.x * a.y * one_minus_x + a.z * y,
                       x + a.y * a.y * one_minus_x,
                       a.y * a.z * one_minus_x - a.x * y,
                       0.0,
                       a.x * a.z * one_minus_x - a.y * y,
                       a.y * a.z * one_minus_x + a.x * y,
                       x + a.z * a.z * one_minus_x,
                       0.0,
                       0.0, 0.0, 0.0, 1.0 };
  gl_matrix_4d_product(m, &r);
  return m;
}
