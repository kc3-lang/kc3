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
#ifndef GL_VERTEX_H
#define GL_VERTEX_H

#include "types.h"

/* Operators. */
void gl_vertex_attrib (void);
void gl_vertex_transform (s_gl_vertex *vertex, const s_mat4 *matrix);

/* Observers. */
sw buf_inspect_gl_vertex (s_buf *buf, const s_gl_vertex *v);
sw err_inspect_gl_vertex (const s_gl_vertex *v);

#endif /* GL_VERTEX_H */
