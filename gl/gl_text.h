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
#ifndef GL_TEXT_H
#define GL_TEXT_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_text_clean
   after use. */
void        gl_text_clean (s_gl_text *text);
s_gl_text * gl_text_init (s_gl_text *text, const s_gl_font *font);
s_gl_text * gl_text_init_1 (s_gl_text *text, const s_gl_font *font,
                            const char *p);
s_gl_text * gl_text_init_str (s_gl_text *text, const s_gl_font *font,
                              const s_str *str);

/* Operators. */
bool gl_text_render_to_texture (s_gl_text *text);
bool gl_text_set_font (s_gl_text *text, const s_gl_font *font);
bool gl_text_set_text (s_gl_text *text, const s_str *str);
bool gl_text_set_text_1 (s_gl_text *text, const char *p);
bool gl_text_set_text_buf (s_gl_text *text, s_buf *buf);
bool gl_text_update (s_gl_text *text);
bool gl_text_update_1 (s_gl_text *text, const char *p);
bool gl_text_update_buf (s_gl_text *text, s_buf *buf);

#endif /* GL_TEXT_H */
