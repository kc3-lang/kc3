/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "mat3.h"
#include "vec2.h"

sw mat3_buf_inspect (s_buf *buf, const s_mat3 *matrix)
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
  while (i < 3) {
    j = 0;
    while (j < 3) {
      if ((r = buf_inspect_f32(buf, m[j * 3 + i])) < 0)
        return r;
      result += r;
      if (i < 2 || j < 2) {
        if ((r = buf_write_1(buf, ",")) < 0)
          return r;
        result += r;
        if (j < 2) {
          if ((r = buf_write_1(buf, " ")) < 0)
            return r;
          result += r;
        }
      }
      j++;
    }
    if (i < 2) {
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

void mat3_delete (s_mat3 *m)
{
  alloc_free(m);
}

s_mat3 * mat3_init_copy (s_mat3 *m, const s_mat3 *src)
{
  assert(m);
  assert(src);
  *m = *src;
  return m;
}

s_mat3 * mat3_init_product (s_mat3 *m, const s_mat3 *a, const s_mat3 *b)
{
  assert(m);
  assert(a);
  assert(b);
  m->xx = a->xx * b->xx + a->xy * b->yx + a->xz * b->zx;
  m->xy = a->xx * b->xy + a->xy * b->yy + a->xz * b->zy;
  m->xz = a->xx * b->xz + a->xy * b->yz + a->xz * b->zz;
  m->yx = a->yx * b->xx + a->yy * b->yx + a->yz * b->zx;
  m->yy = a->yx * b->xy + a->yy * b->yy + a->yz * b->zy;
  m->yz = a->yx * b->xz + a->yy * b->yz + a->yz * b->zz;
  m->zx = a->zx * b->xx + a->zy * b->yx + a->zz * b->zx;
  m->zy = a->zx * b->xy + a->zy * b->yy + a->zz * b->zy;
  m->zz = a->zx * b->xz + a->zy * b->yz + a->zz * b->zz;
  return m;
}

s_mat3 * mat3_init_identity (s_mat3 *m)
{
  assert(m);
  m->xx = 1.0; m->xy = 0.0; m->xz = 0.0;
  m->yx = 0.0; m->yy = 1.0; m->yz = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 1.0;
  return m;
}

s_mat3 * mat3_init_scale (s_mat3 *m, f32 x, f32 y)
{
  assert(m);
  m->xx = x;   m->xy = 0.0; m->xz = 0.0;
  m->yx = 0.0; m->yy = y;   m->yz = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 1.0;
  return m;
}

s_mat3 * mat3_init_translate (s_mat3 *m, f32 x, f32 y)
{
  assert(m);
  m->xx = 1.0; m->xy = 0.0; m->xz = x;
  m->yx = 0.0; m->yy = 1.0; m->yz = y;
  m->zx = 0.0; m->zy = 0.0; m->zz = 1.0;
  return m;
}

s_mat3 * mat3_init_zero (s_mat3 *m)
{
  assert(m);
  m->xx = 0.0; m->xy = 0.0; m->xz = 0.0;
  m->yx = 0.0; m->yy = 0.0; m->yz = 0.0;
  m->zx = 0.0; m->zy = 0.0; m->zz = 0.0;
  return m;
}

s_mat3 * mat3_mult_mat3 (const s_mat3 *a, const s_mat3 *b, s_mat3 *dest)
{
  s_mat3 m;
  assert(a);
  assert(b);
  m.xx = a->xx * b->xx + a->xy * b->yx + a->xz * b->zx;
  m.xy = a->xx * b->xy + a->xy * b->yy + a->xz * b->zy;
  m.xz = a->xx * b->xz + a->xy * b->yz + a->xz * b->zz;
  m.yx = a->yx * b->xx + a->yy * b->yx + a->yz * b->zx;
  m.yy = a->yx * b->xy + a->yy * b->yy + a->yz * b->zy;
  m.yz = a->yx * b->xz + a->yy * b->yz + a->yz * b->zz;
  m.zx = a->zx * b->xx + a->zy * b->yx + a->zz * b->zx;
  m.zy = a->zx * b->xy + a->zy * b->yy + a->zz * b->zy;
  m.zz = a->zx * b->xz + a->zy * b->yz + a->zz * b->zz;
  *dest = m;
  return dest;
}

s_vec2 * mat3_mult_vec2 (const s_mat3 *a, const s_vec2 *b, s_vec2 *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  dest->x = a->xx * b->x + a->xy * b->y + a->xz;
  dest->y = a->yx * b->x + a->yy * b->y + a->yz;
  return dest;
}

s_mat3 * mat3_new_copy (const s_mat3 *src)
{
  s_mat3 *m;
  m = calloc(1, sizeof(s_mat3));
  if (! m) {
    err_puts("mat3_new: failed to allocate memory");
    return NULL;
  }
  *m = *src;
  return m;
}

s_mat3 * mat3_new_product (const s_mat3 *a, const s_mat3 *b)
{
  s_mat3 *m;
  assert(a);
  assert(b);
  m = calloc(1, sizeof(s_mat3));
  if (! m) {
    err_puts("mat3_new: failed to allocate memory");
    return NULL;
  }
  mat3_init_product(m, a, b);
  return m;
}

s_mat3 * mat3_new_zero (void)
{
  s_mat3 *m;
  m = calloc(1, sizeof(s_mat3));
  if (! m) {
    err_puts("mat3_new: failed to allocate memory");
    return NULL;
  }
  mat3_init_zero(m);
  return m;
}

s_mat3 * mat3_scale (s_mat3 *m, f32 x, f32 y)
{
  s_mat3 s;
  mat3_init_identity(&s);
  s.xx = x;
  s.yy = y;
  return mat3_mult_mat3(&s, m, m);
}

s_mat3 * mat3_translate (s_mat3 *m, f32 x, f32 y)
{
  s_mat3 s;
  mat3_init_identity(&s);
  s.zx = x;
  s.zy = y;
  return mat3_mult_mat3(&s, m, m);
}

s_mat3 * mat3_rotate (s_mat3 *m, f32 rad)
{
  f32 c;
  f32 s;
  c = cosf(rad);
  s = sinf(rad);
  s_mat3 r = { c, -s, 0.0,
               s,  c, 0.0,
               0.0, 0.0, 1.0 };
  return mat3_mult_mat3(&r, m, m);
}
