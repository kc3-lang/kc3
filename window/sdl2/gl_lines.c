/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <libkc3/kc3.h>
#include "gl_lines.h"
#include "gl_vertex.h"

s_gl_lines * gl_lines_allocate (s_gl_lines *lines, u32 lines_count)
{
  uw vertex_count;
  assert(lines);
  assert(lines_count);
  vertex_count = lines_count * 2;
  if (! array_init(&lines->vertex, sym_1("GL.Vertex[]"), 1,
                   &vertex_count) ||
      ! array_allocate(&lines->vertex))
    return NULL;
  return lines;
}

void gl_lines_clean (s_gl_lines *lines)
{
  assert(lines);
  array_clean(&lines->vertex);
  glDeleteVertexArrays(1, &lines->gl_vao);
  glDeleteBuffers(1, &lines->gl_vbo);
}

s_gl_lines * gl_lines_init (s_gl_lines *lines)
{
  s_gl_lines tmp = {0};
  assert(glGetError() == GL_NO_ERROR);
  glGenVertexArrays(1, &tmp.gl_vao);
  glGenBuffers(1, &tmp.gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  *lines = tmp;
  return lines;
}

void gl_lines_render (const s_gl_lines *lines, u32 lines_count)
{
  assert(lines);
  if (lines_count > lines->vertex.count / 2)
    lines_count = lines->vertex.count / 2;
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(lines->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(lines->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glDrawArrays(GL_LINES, 0, lines_count * 2);
  assert(glGetError() == GL_NO_ERROR);
}

bool gl_lines_update (s_gl_lines *lines, u32 lines_count)
{
  //GLenum gl_error;
  assert(lines);
  assert(lines->gl_vao);
  assert(lines->gl_vbo);
  assert(lines->vertex.data);
  assert(glGetError() == GL_NO_ERROR);
  if (lines_count > lines->vertex.count / 2)
    lines_count = lines->vertex.count / 2;
  glBindVertexArray(lines->gl_vao);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ARRAY_BUFFER, lines->gl_vbo);
  assert(glGetError() == GL_NO_ERROR);
  glBufferData(GL_ARRAY_BUFFER, lines_count * 2 * sizeof(s_gl_vertex),
               lines->vertex.data, GL_DYNAMIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);
  gl_vertex_attrib();
  assert(glGetError() == GL_NO_ERROR);
  return true;
}
