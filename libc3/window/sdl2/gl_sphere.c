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
#include <assert.h>
#include <math.h>
#include <libc3/c3.h>
#include "gl.h"
#include "gl_sphere.h"

void gl_sphere_clean (s_gl_sphere *sphere)
{
  assert(sphere);
  free(sphere->vertex);
}

void gl_sphere_delete (s_gl_sphere *sphere)
{
  assert(sphere);
  gl_sphere_clean(sphere);
  free(sphere);
}

s_gl_sphere * gl_sphere_init (s_gl_sphere *sphere, uw segments)
{
  f64 angle_i;
  f64 angle_j;
  uw i;
  uw j;
  s_gl_3d *p;
  f64 r;
  f64 z;
  assert(s);
  if (! segments)
    segments = 10;
  sphere->segments = segments;
  p = calloc(segments * segments, sizeof(s_gl_3d));
  if (! p) {
    warn("gl_sphere_init(%lu): point array", segments);
    return NULL;
  }
  sphere->vertex = p;
  i = 0;
  while (i < segments) {
    angle_i = (f64) i / segments * M_PI * 2.0;
    r = cos(angle_i);
    z = sin(angle_i);
    j = 0;
    while (j < segments) {
      angle_j = (f64) j / segments * M_PI * 2.0;
      p->x = cos(angle_j) * r;
      p->y = sin(angle_j) * r;
      p->z = z;
      p++;
      j++;
    }
    i++;
  }
  return sphere;
}

s_gl_sphere * gl_sphere_new (uw segments)
{
  s_gl_sphere *sphere;
  sphere = calloc(1, sizeof(s_gl_sphere));
  if (! sphere) {
    warn("gl_sphere_new: sphere");
    return NULL;
  }
  if (! gl_sphere_init(sphere, segments)) {
    free(sphere);
    return NULL;
  }
  return sphere;
}

void gl_sphere_render (const s_gl_sphere *sphere)
{
  uw i;
  uw j;
  s_gl_3d *p;
  uw seg;
  assert(sphere);
  seg = sphere->segments;
  glBegin(GL_POINTS);
  p = sphere->vertex;
  i = 0;
  while (i < seg) {
    j = 0;
    while (j < seg) {
      gl_vertex_3d(p);
      p++;
      j++;
    }
    i++;
  }
  glEnd();
}
