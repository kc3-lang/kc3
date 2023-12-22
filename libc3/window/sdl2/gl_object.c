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
#include <libc3/c3.h>
#include "gl_object.h"

s_gl_object * gl_object_allocate (s_gl_object *object, uw vertex_count,
                                  uw triangle_count)
{
  assert(object);
  assert(vertex_count);
  assert(triangle_count);
  if (! array_init(&object->vertex, sym_1("GL.Vertex"), 1,
                   &vertex_count) ||
      ! array_allocate(&object->vertex) ||
      ! array_init(&object->triangle, sym_1("GL.Triangle"), 1,
                   &triangle_count) ||
      ! array_allocate(&object->triangle))
    return NULL;
  return object;
}

void gl_object_clean (s_gl_object *object)
{
  assert(object);
  array_clean(&object->vertex);
  array_clean(&object->triangle);
  glDeleteVertexArrays(1, &object->gl_vao);
  glDeleteBuffers(1, &object->gl_vbo);
  glDeleteBuffers(1, &object->gl_ebo);
}

s_gl_object * gl_object_init (s_gl_object *object)
{
  s_gl_object tmp = {0};
  assert(glGetError() == GL_NO_ERROR);
  glGenVertexArrays(1, &tmp.gl_vao);
  glGenBuffers(1, &tmp.gl_vbo);
  glGenBuffers(1, &tmp.gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  *object = tmp;
  return object;
}

void gl_object_render (const s_gl_object *object)
{
  assert(object);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(object->gl_vao);
  glBindBuffer(GL_ARRAY_BUFFER, object->gl_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  assert(glGetError() == GL_NO_ERROR);
}

bool gl_object_update (s_gl_object *object)
{
  assert(object);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(object->gl_vao);
  glBindBuffer(GL_ARRAY_BUFFER, object->gl_vbo);
  glBufferData(GL_ARRAY_BUFFER, object->vertex.size,
               object->vertex.data, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->triangle.size,
               object->triangle.data, GL_STATIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_DOUBLE,   8 * sizeof(f64),
                    (void *) 0);
  glNormalPointer(GL_DOUBLE,      8 * sizeof(f64),
                    (void *) (3 * sizeof(f64)));
  glTexCoordPointer(2, GL_DOUBLE, 8 * sizeof(f64),
                    (void *) (6 * sizeof(f64)));
  assert(glGetError() == GL_NO_ERROR);
  return true;
}
