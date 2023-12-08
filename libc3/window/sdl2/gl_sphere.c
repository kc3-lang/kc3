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

s_gl_sphere * gl_sphere_init (s_gl_sphere *sphere, uw segments_u,
                              uw segments_v)
{
  f64 angle_i;
  f64 angle_j;
  uw i;
  uw j;
  s_gl_3d *p;
  f64 r;
  f64 z;
  assert(s);
  if (! segments_u)
    segments_u = 3;
  if (! segments_v)
    segments_v = 2;
  sphere->segments_u = segments_u;
  sphere->segments_v = segments_v;
  p = calloc(segments_u * segments_v + 2, sizeof(s_gl_3d));
  if (! p) {
    warn("gl_sphere_init(%lu, %lu): point array", segments_u,
         segments_v);
    return NULL;
  }
  sphere->vertex = p;
  i = 0;
  while (i < segments_v) {
    angle_i = (i + 1) * M_PI / (segments_v + 1);
    r = sin(angle_i);
    z = cos(angle_i);
    j = 0;
    while (j < segments_u) {
      angle_j = j * M_PI * 2.0 / segments_u;
      p->x = cos(angle_j) * r;
      p->y = sin(angle_j) * r;
      p->z = z;
      p++;
      j++;
    }
    i++;
  }
  p->x = 0.0;
  p->y = 0.0;
  p->z = 1.0;
  p++;
  p->x = 0.0;
  p->y = 0.0;
  p->z = -1.0;
  return sphere;
}

s_gl_sphere * gl_sphere_new (uw segments_u, uw segments_w)
{
  s_gl_sphere *sphere;
  sphere = calloc(1, sizeof(s_gl_sphere));
  if (! sphere) {
    warn("gl_sphere_new: sphere");
    return NULL;
  }
  if (! gl_sphere_init(sphere, segments_u, segments_w)) {
    free(sphere);
    return NULL;
  }
  return sphere;
}

void gl_sphere_render (const s_gl_sphere *sphere)
{
  uw i;
  uw j;
  s_gl_3d *p[3];
  s_gl_2d tex_coord[3];
  uw seg_u;
  uw seg_v;
  assert(sphere);
  seg_u = sphere->segments_u;
  seg_v = sphere->segments_v;
  // first row
  glBegin(GL_QUAD_STRIP);
  tex_coord[0].y = 0.0;
  tex_coord[1].y = 1.0 / (seg_v + 1);
  p[0] = sphere->vertex + seg_u * seg_v;
  j = 0;
  while (j < seg_u + 1) {
    p[1] = sphere->vertex + j % seg_u;
    tex_coord[0].x = tex_coord[1].x = (f64) j / seg_u;
    gl_tex_coord_2d(tex_coord);
    gl_normal_3d(p[0]);
    gl_vertex_3d(p[0]);
    gl_tex_coord_2d(tex_coord + 1);
    gl_normal_3d(p[1]);
    gl_vertex_3d(p[1]);
    j++;
  }
  glEnd();
  // whole
  i = 1;
  while (i < seg_v) {
    glBegin(GL_QUAD_STRIP);
    tex_coord[0].y = (f64) (i + 1) / (seg_v + 1);
    tex_coord[1].y = (f64) i / (seg_v + 1);
    j = 0;
    while (j < seg_u + 1) {
      p[0] = sphere->vertex +  i      * seg_u + j % seg_u;
      p[1] = sphere->vertex + (i - 1) * seg_u + j % seg_u;
      tex_coord[0].x = tex_coord[1].x = (f64) j / seg_u;
      gl_tex_coord_2d(tex_coord);
      gl_normal_3d(p[0]);
      gl_vertex_3d(p[0]);
      gl_tex_coord_2d(tex_coord + 1);
      gl_normal_3d(p[1]);
      gl_vertex_3d(p[1]);
      j++;
    }
    glEnd();
    i++;
  }
  // last row
  glBegin(GL_QUAD_STRIP);
  tex_coord[0].y = 1.0;
  tex_coord[1].y = (f64) seg_v / (seg_v + 1);
  p[0] = sphere->vertex + seg_u * seg_v + 1;
  j = 0;
  while (j < seg_u + 1) {
    p[1] = sphere->vertex + (seg_v - 1) * seg_u + j % seg_u;
    tex_coord[0].x = tex_coord[1].x = (f64) j / seg_u;
    gl_tex_coord_2d(tex_coord);
    gl_normal_3d(p[0]);
    gl_vertex_3d(p[0]);
    gl_tex_coord_2d(tex_coord + 1);
    gl_normal_3d(p[1]);
    gl_vertex_3d(p[1]);
    j++;
  }
  glEnd();
}

void gl_sphere_render_wireframe (const s_gl_sphere *sphere)
{
  uw i;
  uw j;
  s_gl_3d *p[3];
  uw seg_u;
  uw seg_v;
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
