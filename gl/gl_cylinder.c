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
#include "gl_object.h"
#include "gl_cylinder.h"

void gl_cylinder_clean (s_gl_cylinder *cylinder)
{
  assert(cylinder);
  gl_object_clean(&cylinder->object);
}

s_gl_cylinder * gl_cylinder_init (s_gl_cylinder *cylinder,
                                  u32 segments_u, u32 segments_v)
{
  f64 angle;
  u32 i;
  u32 j;
  s_vec3 *p;
  f64 z;
  assert(cylinder);
  assert(segments_u);
  assert(segments_v);
  cylinder->segments_u = segments_u;
  cylinder->segments_v = segments_v;
  if (! gl_object_init(&cylinder->object) ||
      ! gl_object_allocate(&cylinder->object,
                           segments_u * segments_v + 2,
                           6 * (segments_u + 1) * (segments_v + 2)))
    return NULL;
  p = cylinder->object.vertex.data;
  i = 0;
  while (i < segments_v) {
    z = (f64) i / segments_v;
    j = 0;
    while (j < segments_u) {
      angle = (f64) j / segments_u * M_PI * 2.0;
      p->x = cos(angle);
      p->y = sin(angle);
      p->z = z;
      p++;
      j++;
    }
    i++;
  }
  return cylinder;
}

void gl_cylinder_render (const s_gl_cylinder *cylinder)
{
  assert(cylinder);
  (void) cylinder;
}
