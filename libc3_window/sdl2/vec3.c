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
#include "vec3.h"

s_vec3 * vec3_init (s_vec3 *p, f32 x, f32 y, f32 z)
{
  assert(p);
  p->x = x;
  p->y = y;
  p->z = z;
  return p;
}

s_vec3 * vec3_init_copy (s_vec3 *p, const s_vec3 *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  p->z = src->z;
  return p;
}

s_vec3 * vec3_init_normalize (s_vec3 *p, const s_vec3 *src)
{
  f32 norm;
  f32 ratio;
  assert(p);
  assert(src);
  norm = vec3_norm(src);
  ratio = 1.0f / norm;
  p->x = src->x * ratio;
  p->y = src->y * ratio;
  p->z = src->z * ratio;
  return p;
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

s_vec3 * vec3_init_zero (s_vec3 *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  p->z = 0.0;
  return p;
}

void vec3_delete (s_vec3 *p)
{
  free(p);
}

s_vec3 * vec3_new (f32 x, f32 y, f32 z)
{
  s_vec3 *p;
  p = calloc(1, sizeof(s_vec3));
  if (! p) {
    err_puts("vec3_new: failed to allocate memory");
    return NULL;
  }
  vec3_init(p, x, y, z);
  return p;
}

s_vec3 * vec3_new_copy (const s_vec3 *src)
{
  s_vec3 *p;
  p = calloc(1, sizeof(s_vec3));
  if (! p) {
    err_puts("vec3_new: failed to allocate memory");
    return NULL;
  }
  vec3_init_copy(p, src);
  return p;
}

s_vec3 * vec3_new_zero (void)
{
  s_vec3 *p;
  p = calloc(1, sizeof(s_vec3));
  if (! p) {
    err_puts("vec3_new: failed to allocate memory");
    return NULL;
  }
  vec3_init_zero(p);
  return p;
}

f32 vec3_norm (const s_vec3 *p)
{
  assert(p);
  return sqrtf(p->x * p->x + p->y * p->y + p->z * p->z);
}

void vec3_normalize (s_vec3 *p)
{
  f32 norm;
  f32 ratio;
  assert(p);
  norm = vec3_norm(p);
  ratio = 1.0f / norm;
  p->x *= ratio;
  p->y *= ratio;
  p->z *= ratio;
}
