/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef GL_FONT_H
#define GL_FONT_H

#include "types.h"

extern void *g_ft;

/* Stack-allocation compatible functions, call gl_font_clean
   after use. */
void        gl_font_clean (s_gl_font *font);
s_gl_font * gl_font_init (s_gl_font *font, const char *path,
                          f32 point_per_pixel);

/* Operators. */
void gl_font_set_size (s_gl_font *font, f32 point_size);

#endif /* GL_FONT_H */
