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
#include "gl_object.h"
#include "gl_point_3d.h"
#include "gl_square.h"

void gl_square_clean (s_gl_square *square)
{
  assert(square);
  gl_object_clean(&square->object);
}

void gl_square_delete (s_gl_square *square)
{
  assert(square);
  gl_square_clean(square);
  free(square);
}

s_gl_square * gl_square_init (s_gl_square *square, uw seg_u, uw seg_v)
{
  uw i;
  uw j;
  s_gl_point_3d normal;
  s_gl_square tmp = {0};
  s_gl_triangle *triangle;
  s_gl_vertex *vertex;
  f64 y;
  assert(square);
  if (seg_u < 2)
    seg_u = 2;
  if (seg_v < 2)
    seg_v = 2;
  tmp.segments_u = seg_u;
  tmp.segments_v = seg_v;
  if (! gl_object_init(&tmp.object) ||
      ! gl_object_allocate(&tmp.object, seg_u * seg_v,
                           2 * (seg_u - 1) * (seg_v - 1)))
    return NULL;
  gl_point_3d_init(&normal, 0.0, 0.0, 1.0);
  vertex = tmp.object.vertex.data;
  i = 0;
  while (i < seg_v) {
    y = (f64) i / seg_v;
    j = 0;
    while (j < seg_u) {
      vertex->tex_coord.x = vertex->position.x = (f64) j / seg_u;
      vertex->tex_coord.y = vertex->position.y = y;
      vertex->position.z = 0.0;
      vertex->normal = normal;
      vertex++;
      j++;
    }
    i++;
  }
  triangle = tmp.object.triangle.data;
  i = 0;
  while (i < seg_v - 1) {
    j = 0;
    while (j < seg_u - 1) {
      triangle->a =  i      * seg_u + j;
      triangle->b = (i + 1) * seg_u + j;
      triangle->c = (i + 1) * seg_u + j + 1;
      triangle++;
      triangle->a =  i      * seg_u + j;
      triangle->b = (i + 1) * seg_u + j + 1;
      triangle->c =  i      * seg_u + j + 1;
      triangle++;
      j++;
    }
    i++;
  }
  gl_object_update(&square->object);
  *square = tmp;
  return square;
}

s_gl_square * gl_square_new (uw segments_u, uw segments_v)
{
  s_gl_square *square;
  square = calloc(1, sizeof(s_gl_square));
  if (! square) {
    err_puts("gl_square_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_square_init(square, segments_u, segments_v)) {
    free(square);
    return NULL;
  }
  return square;
}

void gl_square_render (const s_gl_square *square)
{
  assert(square);
  gl_object_render(&square->object);
}

void gl_square_render_wireframe (const s_gl_square *square)
{
  assert(square);
  gl_object_render_wireframe(&square->object);
}
