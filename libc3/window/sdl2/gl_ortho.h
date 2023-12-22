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
#ifndef GL_ORTHO_H
#define GL_ORTHO_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_ortho_clean
   after use. */
void         gl_ortho_clean (s_gl_ortho *ortho);
s_gl_ortho * gl_ortho_init (s_gl_ortho *ortho);

/* Heap-allocation functions, call gl_ortho_delete after use. */
void         gl_ortho_delete (s_gl_ortho *ortho);
s_gl_ortho * gl_ortho_new (void);

/* Operators. */
void         gl_ortho_render (s_gl_ortho *ortho);
void         gl_ortho_render_end (s_gl_ortho *ortho);

#endif /* GL_ORTHO_H */
