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
#include "vec2.h"

s_vec2 * vec2_init (s_vec2 *p, f32 x, f32 y)
{
  assert(p);
  p->x = x;
  p->y = y;
  return p;
}

s_vec2 * vec2_init_copy (s_vec2 *p, const s_vec2 *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  return p;
}

s_vec2 *
vec2_init_product (s_vec2 *p, const s_mat3 *m, const s_vec2 *s)
{
  assert(p);
  assert(m);
  assert(s);
  p->x = s->x * m->xx + s->y * m->xy + m->xz;
  p->y = s->x * m->yx + s->y * m->yy + m->yz;
  return p;
}

s_vec2 * vec2_init_zero (s_vec2 *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  return p;
}

void vec2_delete (s_vec2 *p)
{
  free(p);
}

s_vec2 * vec2_new (f32 x, f32 y)
{
  s_vec2 *p;
  p = calloc(1, sizeof(s_vec2));
  if (! p) {
    err_puts("vec2_new: failed to allocate memory");
    return NULL;
  }
  vec2_init(p, x, y);
  return p;
}

s_vec2 * vec2_new_copy (const s_vec2 *src)
{
  s_vec2 *p;
  p = calloc(1, sizeof(s_vec2));
  if (! p) {
    err_puts("vec2_new: failed to allocate memory");
    return NULL;
  }
  vec2_init_copy(p, src);
  return p;
}

s_vec2 * vec2_new_zero (void)
{
  s_vec2 *p;
  p = calloc(1, sizeof(s_vec2));
  if (! p) {
    err_puts("vec2_new: failed to allocate memory");
    return NULL;
  }
  vec2_init_zero(p);
  return p;
}
