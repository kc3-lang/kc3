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
#ifndef GL_FONT_H
#define GL_FONT_H

#include "types.h"

extern FT_Library g_ft;

/* Stack-allocation compatible functions, call gl_font_clean
   after use. */
void        gl_font_clean (s_gl_font *font);
s_gl_font * gl_font_init (s_gl_font *font, const s8 *path);

/* Operators. */
void gl_font_set_size (s_gl_font *font, f64 size, f64 pixels_per_point);

#endif /* GL_FONT_H */
