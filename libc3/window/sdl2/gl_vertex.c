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
#include "gl_point_3f.h"
#include "gl_vertex.h"

void gl_vertex_attrib (void)
{
  assert(glGetError() == GL_NO_ERROR);
  glEnableVertexAttribArray(0);
  assert(glGetError() == GL_NO_ERROR);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(s_gl_vertex),
                        (void *) 0);
  assert(glGetError() == GL_NO_ERROR);
  glEnableVertexAttribArray(1);
  assert(glGetError() == GL_NO_ERROR);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(s_gl_vertex),
                        (void *) (3 * sizeof(float)));
  assert(glGetError() == GL_NO_ERROR);
  glEnableVertexAttribArray(2);
  assert(glGetError() == GL_NO_ERROR);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(s_gl_vertex),
                        (void *) (6 * sizeof(float)));
  assert(glGetError() == GL_NO_ERROR);
}

void gl_vertex_transform (s_gl_vertex *vertex,
                          const s_gl_matrix_4f *matrix)
{
  assert(vertex);
  assert(matrix);
  gl_point_3f_transform(&vertex->position, matrix);
  gl_point_3f_transform(&vertex->normal, matrix);
  gl_point_3f_normalize(&vertex->normal);
}
