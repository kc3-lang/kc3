/* kc3
 * Copyright 2022-2025 kmx.io <contact@kmx.io>
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
#include <libkc3/kc3.h>
#include "gl_object.h"
#include "gl_box.h"

void gl_box_clean (s_gl_box *box)
{
  assert(box);
  gl_object_clean(&box->object);
}

void gl_box_delete (s_gl_box *box)
{
  assert(box);
  gl_box_clean(box);
  free(box);
}

s_gl_box * gl_box_init (s_gl_box *box, u32 seg_u, u32 seg_v, u32 seg_w)
{
  u32 i;
  u32 j;
  u32 k;
  f32 t;
  u32 v;
  s_gl_vertex *vertex;
  s_gl_box tmp = {0};
  s_gl_triangle *triangle;
  assert(box);
  if (seg_u < 1)
    seg_u = 1;
  if (seg_v < 1)
    seg_v = 1;
  if (seg_w < 1)
    seg_w = 1;
  tmp.segments_u = seg_u;
  tmp.segments_v = seg_v;
  tmp.segments_w = seg_w;
  if (! gl_object_init(&tmp.object) ||
      ! gl_object_allocate(&tmp.object,
                           2 * (seg_u + 1) * (seg_v + 1) +
                           2 * (seg_v + 1) * (seg_w + 1) +
                           2 * (seg_w + 1) * (seg_u + 1),
                           4 * seg_u * seg_v +
                           4 * seg_v * seg_w +
                           4 * seg_w * seg_u))
    return NULL;
  vertex = tmp.object.vertex.data;
  k = 0;
  t = -0.5;
  while (k < 2) {
    j = 0;
    while (j < seg_v + 1) {
      i = 0;
      while (i < seg_u + 1) {
        vertex->pos_x = (f32) i / seg_u - 0.5f;
        vertex->pos_y = (f32) j / seg_v - 0.5f;
        vertex->pos_z = t;
        vertex->normal_x = 0.0f;
        vertex->normal_y = 0.0f;
        vertex->normal_z = t * 2.0f;
        vertex->tex_coord_x = (f32) i / seg_u;
        vertex->tex_coord_y = (f32) j / seg_v;
        vertex++;
        i++;
      }
      j++;
    }
    t += 1.0f;
    k++;
  }
  k = 0;
  t = -0.5f;
  while (k < 2) {
    j = 0;
    while (j < seg_w + 1) {
      i = 0;
      while (i < seg_v + 1) {
        vertex->pos_x = k;
        vertex->pos_y = (f32) i / seg_v - 0.5f;
        vertex->pos_z = (f32) j / seg_w - 0.5f;
        vertex->normal_x = k * 2.0f;
        vertex->normal_y = 0.0f;
        vertex->normal_z = 0.0f;
        vertex->tex_coord_x = (f32) i / seg_v;
        vertex->tex_coord_y = (f32) j / seg_w;
        vertex++;
        i++;
      }
      j++;
    }
    t += 1.0f;
    k++;
  }
  k = 0;
  t = -0.5f;
  while (k < 2) {
    j = 0;
    while (j < seg_u + 1) {
      i = 0;
      while (i < seg_w + 1) {
        vertex->pos_x = (f32) j / seg_u - 0.5f;
        vertex->pos_y = k;
        vertex->pos_z = (f32) i / seg_w - 0.5f;
        vertex->normal_x = 0.0f;
        vertex->normal_y = k * 2.0f;
        vertex->normal_z = 0.0f;
        vertex->tex_coord_x = (f32) i / seg_w;
        vertex->tex_coord_y = (f32) j / seg_u;
        vertex++;
        i++;
      }
      j++;
    }
    k++;
  }
  v = 0;
  triangle = tmp.object.triangle.data;
  k = 0;
  while (k < 2) {
    j = 0;
    while (j < seg_v) {
      i = 0;
      while (i < seg_u) {
        triangle->a = v +  i      * (seg_u + 1) + j;
        triangle->b = v + (i + 1) * (seg_u + 1) + j;
        triangle->c = v + (i + 1) * (seg_u + 1) + j + 1;
        triangle++;
        triangle->a = v +  i      * (seg_u + 1) + j;
        triangle->b = v + (i + 1) * (seg_u + 1) + j + 1;
        triangle->c = v +  i      * (seg_u + 1) + j + 1;
        triangle++;
        i++;
      }
      j++;
    }
    v += (seg_u + 1) * (seg_v + 1);
    k++;
  }
  k = 0;
  while (k < 2) {
    j = 0;
    while (j < seg_w) {
      i = 0;
      while (i < seg_v) {
        triangle->a = v +  i      * (seg_v + 1) + j;
        triangle->b = v + (i + 1) * (seg_v + 1) + j;
        triangle->c = v + (i + 1) * (seg_v + 1) + j + 1;
        triangle++;
        triangle->a = v +  i      * (seg_v + 1) + j;
        triangle->b = v + (i + 1) * (seg_v + 1) + j + 1;
        triangle->c = v +  i      * (seg_v + 1) + j + 1;
        triangle++;
        i++;
      }
      j++;
    }
    v += (seg_v + 1) * (seg_w + 1);
    k++;
  }
  k = 0;
  while (k < 2) {
    j = 0;
    while (j < seg_u) {
      i = 0;
      while (i < seg_w) {
        triangle->a = v +  i      * (seg_w + 1) + j;
        triangle->b = v + (i + 1) * (seg_w + 1) + j;
        triangle->c = v + (i + 1) * (seg_w + 1) + j + 1;
        triangle++;
        triangle->a = v +  i      * (seg_w + 1) + j;
        triangle->b = v + (i + 1) * (seg_w + 1) + j + 1;
        triangle->c = v +  i      * (seg_w + 1) + j + 1;
        triangle++;
        i++;
      }
      j++;
    }
    v += (seg_w + 1) * (seg_u + 1);
    k++;
  }
  *box = tmp;
  return box;
}

s_gl_box * gl_box_new (u32 seg_u, u32 seg_v, u32 seg_w)
{
  s_gl_box *box;
  box = calloc(1, sizeof(s_gl_box));
  if (! box) {
    err_puts("gl_box_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_box_init(box, seg_u, seg_v, seg_w)) {
    free(box);
    return NULL;
  }
  return box;
}

void gl_box_render (const s_gl_box *box)
{
  assert(box);
  gl_object_render(&box->object);
}


void gl_box_render_wireframe (const s_gl_box *box)
{
  assert(box);
  gl_object_render_wireframe(&box->object);
}
