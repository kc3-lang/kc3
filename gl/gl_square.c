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
#include "vec3.h"
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

s_gl_square * gl_square_init (s_gl_square *square, u32 seg_u, u32 seg_v)
{
  u32 i;
  u32 j;
  s_gl_square tmp = {0};
  s_gl_triangle *triangle;
  s_gl_vertex *vertex;
  f32 y;
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
  vertex = tmp.object.vertex.data;
  i = 0;
  while (i < seg_v) {
    y = (f32) i / (seg_v - 1);
    j = 0;
    while (j < seg_u) {
      vertex->tex_coord_x = vertex->pos_x = (f32) j / (seg_u - 1);
      vertex->tex_coord_y = vertex->pos_y = y;
      vertex->pos_z = 0.0;
      vertex->normal_x = 0.0;
      vertex->normal_y = 0.0;
      vertex->normal_z = 1.0;
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
  gl_object_update(&tmp.object);
  *square = tmp;
  return square;
}

s_gl_square * gl_square_new (u32 segments_u, u32 segments_v)
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
