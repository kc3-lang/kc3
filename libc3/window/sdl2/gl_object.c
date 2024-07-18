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
#include "gl_deprecated.h"
#include "gl_object.h"
#include "gl_vertex.h"

s_gl_object * gl_object_allocate (s_gl_object *object, uw vertex_count,
                                  uw triangle_count)
{
  assert(object);
  assert(vertex_count);
  assert(triangle_count);
  if (! array_init(&object->vertex, sym_1("GL.Vertex[]"), 1,
                   &vertex_count) ||
      ! array_allocate(&object->vertex) ||
      ! array_init(&object->triangle, sym_1("GL.Triangle[]"), 1,
                   &triangle_count) ||
      ! array_allocate(&object->triangle))
    return NULL;
  return object;
}

void gl_object_clean (s_gl_object *object)
{
  assert(object);
  assert(glGetError() == GL_NO_ERROR);
  if (object->gl_vao)
    glDeleteVertexArrays(1, &object->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  if (object->gl_vbo)
    glDeleteBuffers(1, &object->gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  if (object->gl_ebo)
    glDeleteBuffers(1, &object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  array_clean(&object->vertex);
  array_clean(&object->triangle);
}

s_gl_object * gl_object_init (s_gl_object *object)
{
  s_gl_object tmp = {0};
  assert(glGetError() == GL_NO_ERROR);
  glGenVertexArrays(1, &tmp.gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glGenBuffers(1, &tmp.gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  glGenBuffers(1, &tmp.gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  *object = tmp;
  return object;
}

void gl_object_render (const s_gl_object *object)
{
  GLenum error;
  assert(object);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(object->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ARRAY_BUFFER, object->gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  glDrawElements(GL_TRIANGLES, object->triangle.count * 3, GL_UNSIGNED_INT,
                 NULL);
  if ((error = glGetError()) != GL_NO_ERROR) {
    err_write_1("gl_object_render: glDrawElements: ");
    err_puts(gl_error_string(error));
    assert(! "gl_object_render: glDrawElements");
  }
  assert(glGetError() == GL_NO_ERROR);
}

void gl_object_render_wireframe (const s_gl_object *object)
{
  assert(object);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(object->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ARRAY_BUFFER, object->gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  glDrawElements(GL_LINE_LOOP, object->triangle.count * 3,
                 GL_UNSIGNED_INT,
                 NULL);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_object_transform (s_gl_object *object, const s_mat4 *matrix)
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
               object->vertex.data, GL_DYNAMIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);
  gl_vertex_attrib();
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gl_ebo);
  assert(glGetError() == GL_NO_ERROR);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->triangle.count * sizeof(s_gl_triangle),
               object->triangle.data, GL_DYNAMIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}
