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
#include "gl_vertex.h"

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
  //glBindVertexArray(object->gl_vao);
  //assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ARRAY_BUFFER, object->gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  glDrawArrays(GL_TRIANGLES, 0, object->triangle.count * 3);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_object_render_wireframe (const s_gl_object *object)
{
  assert(object);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(object->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  glDrawElements(GL_LINE_LOOP, object->triangle.count * 3, GL_UNSIGNED_INT, 0);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_object_transform (s_gl_object *object,
                          const s_gl_matrix_4f *matrix)
{
  uw i;
  s_gl_vertex *vertex;
  assert(object);
  assert(matrix);
  vertex = object->vertex.data;
  i = 0;
  while (i < object->vertex.count) {
    gl_vertex_transform(vertex, matrix);
    vertex++;
    i++;
  }
}

bool gl_object_update (s_gl_object *object)
{
  //GLenum gl_error;
  assert(object);
  assert(object->gl_vao);
  assert(object->gl_vbo);
  assert(object->gl_ebo);
  assert(object->vertex.data);
  assert(object->triangle.data);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(object->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ARRAY_BUFFER, object->gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  glBufferData(GL_ARRAY_BUFFER, object->vertex.count * sizeof(s_gl_vertex),
               object->vertex.data, GL_STATIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(s_gl_vertex),
                        (void *) 0);
  assert(glGetError() == GL_NO_ERROR);
  glEnableVertexAttribArray(0);
  assert(glGetError() == GL_NO_ERROR);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(s_gl_vertex),
                        (void *) (3 * sizeof(float)));
  assert(glGetError() == GL_NO_ERROR);
  glEnableVertexAttribArray(1);
  assert(glGetError() == GL_NO_ERROR);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(s_gl_vertex),
                        (void *) (6 * sizeof(float)));
  assert(glGetError() == GL_NO_ERROR);
  glEnableVertexAttribArray(2);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->triangle.count * 3,
               object->triangle.data, GL_STATIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}
