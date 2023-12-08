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
#include "gl.h"

void gl_normal_3d (const s_gl_3d *normal)
{
  glNormal3dv(&normal->x);
}

void gl_tex_coord_2d (const s_gl_2d *tex_coord)
{
  glTexCoord2dv(&tex_coord->x);
}

void gl_vertex_3d (const s_gl_3d *vertex)
{
  glVertex3dv(&vertex->x);
}
