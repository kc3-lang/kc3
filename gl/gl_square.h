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
#ifndef GL_SQUARE_H
#define GL_SQUARE_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_square_clean after
   use. */
void          gl_square_clean (s_gl_square *square);
s_gl_square * gl_square_init (s_gl_square *square, u32 segments_u,
                              u32 segments_v);

/* Heap-allocation functions, call gl_square_delete after use. */
void          gl_square_delete (s_gl_square *square);
s_gl_square * gl_square_new (u32 segments_u, u32 segments_v);

/* Observers. */
void          gl_square_render (const s_gl_square *square);
void          gl_square_render_wireframe (const s_gl_square *square);

#endif /* GL_SQUARE_H */
