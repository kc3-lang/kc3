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
#include "dvec2.h"

s_dvec2 * dvec2_init (s_dvec2 *p, f64 x, f64 y)
{
  assert(p);
  p->x = x;
  p->y = y;
  return p;
}

s_dvec2 * dvec2_init_copy (s_dvec2 *p, const s_dvec2 *src)
{
  assert(p);
  assert(src);
  p->x = src->x;
  p->y = src->y;
  return p;
}

s_dvec2 * dvec2_init_product (s_dvec2 *p, const s_dmat3 *m,
                              const s_dvec2 *s)
{
  assert(p);
  assert(m);
  assert(s);
  p->x = s->x * m->xx + s->y * m->xy + m->xz;
  p->y = s->x * m->yx + s->y * m->yy + m->yz;
  return p;
}

s_dvec2 * dvec2_init_zero (s_dvec2 *p)
{
  assert(p);
  p->x = 0.0;
  p->y = 0.0;
  return p;
}

void dvec2_delete (s_dvec2 *p)
{
  free(p);
}

s_dvec2 * dvec2_new (f64 x, f64 y)
{
  s_dvec2 *p;
  p = calloc(1, sizeof(s_dvec2));
  if (! p) {
    err_puts("dvec2_new: failed to allocate memory");
    return NULL;
  }
  dvec2_init(p, x, y);
  return p;
}

s_dvec2 * dvec2_new_copy (const s_dvec2 *src)
{
  s_dvec2 *p;
  p = calloc(1, sizeof(s_dvec2));
  if (! p) {
    err_puts("dvec2_new: failed to allocate memory");
    return NULL;
  }
  dvec2_init_copy(p, src);
  return p;
}

s_dvec2 * dvec2_new_zero (void)
{
  s_dvec2 *p;
  p = calloc(1, sizeof(s_dvec2));
  if (! p) {
    err_puts("dvec2_new: failed to allocate memory");
    return NULL;
  }
  dvec2_init_zero(p);
  return p;
}
