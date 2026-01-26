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
void         gl_ortho_bind_texture (s_gl_ortho *ortho, u32 texture);
void         gl_ortho_color (s_gl_ortho *ortho, f32 r, f32 g, f32 b,
                             f32 a);
void         gl_ortho_rect (s_gl_ortho *ortho, f32 x1, f32 x2, f32 y1,
                            f32 y2);
void         gl_ortho_render (s_gl_ortho *ortho);
void         gl_ortho_render_end (s_gl_ortho *ortho);
void         gl_ortho_resize (s_gl_ortho *ortho, f32 x1, f32 x2, f32 y1,
                              f32 y2, f32 clip_z_near, f32 clip_z_far);
void         gl_ortho_text_render (s_gl_ortho *ortho,
                                   const s_gl_text *text);
void         gl_ortho_text_render_outline (s_gl_ortho *ortho,
                                           s_gl_text *text,
                                           f64 x, f64 y,
                                           const s_rgb *outline_color,
                                           const s_rgb *color);
void         gl_ortho_update_model_matrix (s_gl_ortho *ortho);
void         gl_ortho_update_view_matrix (s_gl_ortho *ortho);
void         gl_ortho_vtext_render (s_gl_ortho *ortho,
                                    const s_gl_text *text);

#endif /* GL_ORTHO_H */
