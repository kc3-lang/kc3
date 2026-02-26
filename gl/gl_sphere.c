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
#include "gl_sphere.h"

void gl_sphere_clean (s_gl_sphere *sphere)
{
  assert(sphere);
  gl_object_clean(&sphere->object);
}

void gl_sphere_delete (s_gl_sphere *sphere)
{
  assert(sphere);
  gl_sphere_clean(sphere);
  alloc_free(sphere);
}

s_gl_sphere * gl_sphere_init (s_gl_sphere *sphere, u32 seg_u, u32 seg_v)
{
  f64 angle_i;
  f64 angle_j;
  u32 i;
  u32 j;
  s_gl_vertex *vertex;
  f64 r;
  s_gl_sphere tmp = {0};
  s_gl_triangle *triangle;
  f64 z;
  assert(sphere);
  if (seg_u < 3)
    seg_u = 3;
  if (seg_v < 1)
    seg_v = 1;
  tmp.segments_u = seg_u;
  tmp.segments_v = seg_v;
  if (! gl_object_init(&tmp.object) ||
      ! gl_object_allocate(&tmp.object, (seg_u + 1) * (seg_v + 2),
                           2 * seg_u * (seg_v + 1)))
    return NULL;
  vertex = tmp.object.vertex.data;
  i = 0;
  while (i <= seg_v + 1) {
    angle_i = (f64) i * M_PI / (seg_v + 1);
    r = sin(angle_i);
    z = cos(angle_i);
    j = 0;
    while (j <= seg_u) {
      angle_j = j * M_PI * 2.0 / seg_u;
      vertex->pos_x = cos(angle_j) * r;
      vertex->pos_y = sin(angle_j) * r;
      vertex->pos_z = z;
      vertex->normal_x = vertex->pos_x;
      vertex->normal_y = vertex->pos_y;
      vertex->normal_z = vertex->pos_z;
      vertex->tex_coord_x = (f64) (seg_u - j) / seg_u;
      vertex->tex_coord_y = (f64) i / (seg_v + 1);
      vertex++;
      j++;
    }
    i++;
  }
  triangle = tmp.object.triangle.data;
  i = 0;
  while (i < seg_v + 1) {
    j = 0;
    while (j < seg_u) {
      triangle->a =  i      * (seg_u + 1) + j;
      triangle->b = (i + 1) * (seg_u + 1) + j;
      triangle->c = (i + 1) * (seg_u + 1) + j + 1;
      triangle++;
      triangle->a =  i      * (seg_u + 1) + j;
      triangle->b = (i + 1) * (seg_u + 1) + j + 1;
      triangle->c =  i      * (seg_u + 1) + j + 1;
      triangle++;
      j++;
    }
    i++;
  }
  *sphere = tmp;
  return sphere;
}

s_gl_sphere * gl_sphere_new (u32 segments_u, u32 segments_v)
{
  s_gl_sphere *sphere;
  sphere = calloc(1, sizeof(s_gl_sphere));
  if (! sphere) {
    err_puts("gl_sphere_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_sphere_init(sphere, segments_u, segments_v)) {
    alloc_free(sphere);
    return NULL;
  }
  return sphere;
}

void gl_sphere_render (const s_gl_sphere *sphere)
{
  assert(sphere);
  gl_object_render(&sphere->object);
}

/*
void gl_sphere_render_wireframe (const s_gl_sphere *sphere)
{
  u32 i;
  u32 j;
  s_gl_3d *p[3];
  u32 seg_u;
  u32 seg_v;
  assert(sphere);
  seg_u = sphere->segments_u;
  seg_v = sphere->segments_v;
  // first row
  j = 0;
  while (j < seg_u + 1) {
    glBegin(GL_LINE_STRIP);
    p[0] = sphere->vertex + seg_u * seg_v;
    p[1] = sphere->vertex +  j      % seg_u;
    p[2] = sphere->vertex + (j + 1) % seg_u;
    gl_vertex_3d(p[0]);
    gl_vertex_3d(p[1]);
    gl_vertex_3d(p[2]);
    glEnd();
    j++;
  }
  // whole
  i = 1;
  while (i < seg_v) {
    j = 0;
    while (j < seg_u + 1) {
      glBegin(GL_LINE_STRIP);
      p[0] = sphere->vertex + (i - 1) * seg_u +  j      % seg_u;
      p[1] = sphere->vertex +  i      * seg_u +  j      % seg_u;
      p[2] = sphere->vertex +  i      * seg_u + (j + 1) % seg_u;
      gl_vertex_3d(p[0]);
      gl_vertex_3d(p[1]);
      gl_vertex_3d(p[2]);
      glEnd();
      j++;
    }
    i++;
  }
  glBegin(GL_LINES);
  j = 0;
  while (j < seg_u + 1) {
    p[0] = sphere->vertex + seg_u * seg_v + 1;
    p[1] = sphere->vertex + (seg_v - 1) * seg_u + j % seg_u;
    gl_vertex_3d(p[0]);
    gl_vertex_3d(p[1]);
    j++;
  }
  glEnd();
}
*/
