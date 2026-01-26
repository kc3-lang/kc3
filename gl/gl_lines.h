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
#ifndef GL_LINES_H
#define GL_LINES_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_lines_clean
   after use. */
void         gl_lines_clean (s_gl_lines *lines);
s_gl_lines * gl_lines_init (s_gl_lines *lines);

/* Heap-allocation functions, call gl_lines_delete after use. */
void         gl_lines_delete (s_gl_lines *lines);
s_gl_lines * gl_lines_new (void);

/* Operators. */
s_gl_lines * gl_lines_allocate (s_gl_lines *lines, u32 lines_count);
bool         gl_lines_update (s_gl_lines *lines, u32 lines_count);

/* Observers. */
void         gl_lines_render (const s_gl_lines *lines, u32 lines_count);
void         gl_lines_render_wireframe (const s_gl_lines *lines,
                                        u32 lines_count);

#endif /* GL_LINES_H */
