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
#ifndef GL_VTEXT_H
#define GL_VTEXT_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_vtext_clean
   after use. */
void        gl_vtext_clean (s_gl_text *text);
s_gl_text * gl_vtext_init (s_gl_text *text, const s_gl_font *font);
s_gl_text * gl_vtext_init_1 (s_gl_text *text, const s_gl_font *font,
                            const char *p);
s_gl_text * gl_vtext_init_str (s_gl_text *text, const s_gl_font *font,
                              const s_str *str);

/* Heap-allocation functions, call gl_vtext_delete after use. */
void        gl_vtext_delete (s_gl_text *text);
s_gl_text * gl_vtext_new (const s_gl_font *font);
s_gl_text * gl_vtext_new_1 (const s_gl_font *font, const char *p);
s_gl_text * gl_vtext_new_str (const s_gl_font *font, const s_str *str);

/* Operators. */
bool gl_vtext_render_to_texture (s_gl_text *text);
bool gl_vtext_render_to_texture_random (s_gl_text *text, u32 len);
bool gl_vtext_set_font (s_gl_text *text, const s_gl_font *font);
bool gl_vtext_set_text (s_gl_text *text, const s_str *str);
bool gl_vtext_set_text_1 (s_gl_text *text, const char *p);
bool gl_vtext_set_text_buf (s_gl_text *text, s_buf *buf);
bool gl_vtext_update (s_gl_text *text);
bool gl_vtext_update_1 (s_gl_text *text, const char *p);
bool gl_vtext_update_buf (s_gl_text *text, s_buf *buf);

#endif /* GL_TEXT_H */
