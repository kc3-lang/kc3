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
#ifndef GL_BOX_H
#define GL_BOX_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_box_clean after
   use. */
void       gl_box_clean (s_gl_box *box);
s_gl_box * gl_box_init (s_gl_box *box, u32 segments_u,
                        u32 segments_v, u32 segments_w);

/* Heap-allocation functions, call gl_box_delete after use. */
void       gl_box_delete (s_gl_box *box);
s_gl_box * gl_box_new (u32 segments_u, u32 segments_v,
                       u32 segments_w);

/* Observers. */
void gl_box_render (const s_gl_box *box);
void gl_box_render_points (const s_gl_box *box);
void gl_box_render_triangles (const s_gl_box *box);
void gl_box_render_wireframe (const s_gl_box *box);

#endif /* GL_BOX_H */
