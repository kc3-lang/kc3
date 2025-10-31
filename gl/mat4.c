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
#include "mat4.h"
#include "vec3.h"

sw mat4_buf_inspect (s_buf *buf, const s_mat4 *matrix)
{
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(buf);
  assert(matrix);
  const f32 *m;
  if ((r = buf_write_1(buf, "(F32) {")) < 0)
    return r;
  result += r;
  m = &matrix->xx;
  i = 0;
  while (i < 4) {
    j = 0;
    while (j < 4) {
      if ((r = buf_inspect_f32(buf, m[j * 4 + i])) < 0)
        return r;
      result += r;
      if (i < 3 || j < 3) {
        if ((r = buf_write_1(buf, ",")) < 0)
          return r;
        result += r;
        if (j < 3) {
          if ((r = buf_write_1(buf, " ")) < 0)
            return r;
          result += r;
        }
      }
      j++;
    }
    if (i < 3) {
      if ((r = buf_write_1(buf, "\n       ")) < 0)
        return r;
      result += r;
    }
    i++;
  }
  if ((r = buf_write_1(buf, "}\n")) < 0)
    return r;
  result += r;
  return result;
}

void mat4_delete (s_mat4 *m)
{
  free(m);
}

s_mat4 * mat4_init_copy (s_mat4 *m, const s_mat4 *src)
{
  assert(m);
  assert(src);
  *m = *src;
  return m;
}

s_mat4 * mat4_init_ortho (s_mat4 *m, f32 x1, f32 x2, f32 y1, f32 y2,
                          f32 clip_z_near, f32 clip_z_far)
{
  f32 dx;
  f32 dy;
  f32 dz;
  s_mat4 ortho;
  assert(m);
  dx = x2 - x1;
  dy = y2 - y1;
  dz = clip_z_far - clip_z_near;
  mat4_init_zero(&ortho);
  ortho.xx = 2.0 / dx;
  ortho.yy = 2.0 / dy;
  ortho.zz = -2.0 / dz;
  ortho.tx = - (x1 + x2) / dx;
  ortho.ty = - (y1 + y2) / dy;
  ortho.tz = - (clip_z_near + clip_z_far) / dz;
  ortho.tt = 1.0;
  *m = ortho;
  return m;
}

s_mat4 * mat4_init_product (s_mat4 *m, const s_mat4 *a, const s_mat4 *b)
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

s_mat4 * mat4_init_identity (s_mat4 *m)
{
  assert(m);
  m->xx = 1.0; m->xy = 0.0; m->xz = 0.0; m->xt = 0.0;
  m->yx = 0.0; m->yy = 1.0; m->yz = 0.0; m->yt = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 1.0; m->zt = 0.0;
  m->tx = 0.0; m->ty = 0.0; m->tz = 0.0; m->tt = 1.0;
  return m;
}

s_mat4 * mat4_init_scale (s_mat4 *m, f32 x, f32 y, f32 z)
{
  assert(m);
  m->xx = x;   m->xy = 0.0; m->xz = 0.0; m->xt = 0.0;
  m->yx = 0.0; m->yy = y;   m->yz = 0.0; m->yt = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = z;   m->zt = 0.0;
  m->tx = 0.0; m->ty = 0.0; m->tz = 0.0; m->tt = 1.0;
  return m;
}

s_mat4 * mat4_init_translate (s_mat4 *m, f32 x, f32 y, f32 z)
{
  assert(m);
  m->xx = 1.0; m->xy = 0.0; m->xz = 0.0; m->xt = x;
  m->yx = 0.0; m->yy = 1.0; m->yz = 0.0; m->yt = y;
  m->zx = 0.0; m->zy = 0.0; m->zz = 1.0; m->zt = z;
  m->tx = 0.0; m->ty = 0.0; m->tz = 0.0; m->tt = 1.0;
  return m;
}

s_mat4 * mat4_init_zero (s_mat4 *m)
{
  assert(m);
  m->xx = 0.0; m->xy = 0.0; m->xz = 0.0; m->xt = 0.0;
  m->yx = 0.0; m->yy = 0.0; m->yz = 0.0; m->yt = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 0.0; m->zt = 0.0;
  m->tx = 0.0; m->ty = 0.0; m->tz = 0.0; m->tt = 0.0;
  return m;
}

s_mat4 * mat4_mult_mat4 (const s_mat4 *a, const s_mat4 *b, s_mat4 *dest)
{
  s_mat4 m;
  assert(a);
  assert(b);
  m.xx = a->xx * b->xx + a->xy * b->yx + a->xz * b->zx + a->xt * b->tx;
  m.xy = a->xx * b->xy + a->xy * b->yy + a->xz * b->zy + a->xt * b->ty;
  m.xz = a->xx * b->xz + a->xy * b->yz + a->xz * b->zz + a->xt * b->tz;
  m.xt = a->xx * b->xt + a->xy * b->yt + a->xz * b->zt + a->xt * b->tt;
  m.yx = a->yx * b->xx + a->yy * b->yx + a->yz * b->zx + a->yt * b->tx;
  m.yy = a->yx * b->xy + a->yy * b->yy + a->yz * b->zy + a->yt * b->ty;
  m.yz = a->yx * b->xz + a->yy * b->yz + a->yz * b->zz + a->yt * b->tz;
  m.yt = a->yx * b->xt + a->yy * b->yt + a->yz * b->zt + a->yt * b->tt;
  m.zx = a->zx * b->xx + a->zy * b->yx + a->zz * b->zx + a->zt * b->tx;
  m.zy = a->zx * b->xy + a->zy * b->yy + a->zz * b->zy + a->zt * b->ty;
  m.zz = a->zx * b->xz + a->zy * b->yz + a->zz * b->zz + a->zt * b->tz;
  m.zt = a->zx * b->xt + a->zy * b->yt + a->zz * b->zt + a->zt * b->tt;
  m.tx = a->tx * b->xx + a->ty * b->yx + a->tz * b->zx + a->tt * b->tx;
  m.ty = a->tx * b->xy + a->ty * b->yy + a->tz * b->zy + a->tt * b->ty;
  m.tz = a->tx * b->xz + a->ty * b->yz + a->tz * b->zz + a->tt * b->tz;
  m.tt = a->tx * b->xt + a->ty * b->yt + a->tz * b->zt + a->tt * b->tt;
  *dest = m;
  return dest;
}

s_vec3 * mat4_mult_vec3 (const s_mat4 *a, const s_vec3 *b, s_vec3 *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  dest->x = a->xx * b->x + a->xy * b->y + a->xz * b->z + a->xt;
  dest->y = a->yx * b->x + a->yy * b->y + a->yz * b->z + a->yt;
  dest->z = a->zx * b->x + a->zy * b->y + a->zz * b->z + a->zt;
  return dest;
}

s_mat4 * mat4_new_copy (const s_mat4 *src)
{
  s_mat4 *m;
  m = calloc(1, sizeof(s_mat4));
  if (! m) {
    err_puts("mat4_new: failed to allocate memory");
    return NULL;
  }
  *m = *src;
  return m;
}

s_mat4 * mat4_new_product (const s_mat4 *a, const s_mat4 *b)
{
  s_mat4 *m;
  assert(a);
  assert(b);
  m = calloc(1, sizeof(s_mat4));
  if (! m) {
    err_puts("mat4_new: failed to allocate memory");
    return NULL;
  }
  mat4_init_product(m, a, b);
  return m;
}

s_mat4 * mat4_new_zero (void)
{
  s_mat4 *m;
  m = calloc(1, sizeof(s_mat4));
  if (! m) {
    err_puts("mat4_new: failed to allocate memory");
    return NULL;
  }
  mat4_init_zero(m);
  return m;
}

s_mat4 * mat4_ortho (s_mat4 *m, f32 x1, f32 x2, f32 y1, f32 y2,
                     f32 clip_z_near, f32 clip_z_far)
{
  s_mat4 ortho;
  assert(m);
  mat4_init_ortho(&ortho, x1, x2, y1, y2, clip_z_near, clip_z_far);
  return mat4_mult_mat4(&ortho, m, m);
}

s_mat4 * mat4_perspective (s_mat4 *m, f32 fov_y,
                                           f32 aspect_ratio,
                                           f32 z_near,
                                           f32 z_far)
{
  f32 dz;
  s_mat4 perspective;
  f32 f;
  f32 fov_y_2;
  fov_y_2 = fov_y / 2.0;
  f = cos(fov_y_2) / sin(fov_y_2);
  dz = z_near - z_far;
  mat4_init_zero(&perspective);
  perspective.xx = f / aspect_ratio;
  perspective.yy = f;
  perspective.zz = (z_near + z_far) / dz;
  perspective.zt = -1.0;
  perspective.tz = 2.0 * z_near * z_far / dz;
  return mat4_mult_mat4(&perspective, m, m);
}

s_mat4 * mat4_scale (s_mat4 *m, f32 x, f32 y, f32 z)
{
  s_mat4 s;
  mat4_init_identity(&s);
  s.xx = x;
  s.yy = y;
  s.zz = z;
  return mat4_mult_mat4(&s, m, m);
}

s_mat4 * mat4_translate (s_mat4 *m, f32 x, f32 y, f32 z)
{
  s_mat4 s;
  mat4_init_identity(&s);
  s.tx = x;
  s.ty = y;
  s.tz = z;
  return mat4_mult_mat4(&s, m, m);
}

s_mat4 * mat4_rotate_axis (s_mat4 *m, f32 rad, const s_vec3 *axis)
{
  s_vec3 a;
  f32 angle;
  f32 one_minus_x;
  f32 x;
  f32 y;
  vec3_init_normalize(&a, axis);
  angle = -rad;
  x = cosf(angle);
  one_minus_x = 1.0 - x;
  y = sinf(angle);
  s_mat4 r = { x + a.x * a.x * one_minus_x,
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
  return mat4_mult_mat4(&r, m, m);
}
