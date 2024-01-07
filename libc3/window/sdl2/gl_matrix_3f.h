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
#ifndef GL_MATRIX_3F_H
#define GL_MATRIX_3F_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_matrix_3f * gl_matrix_3f_init_copy (s_gl_matrix_3f *m,
                                         const s_gl_matrix_3f *src);
s_gl_matrix_3f * gl_matrix_3f_init_identity (s_gl_matrix_3f *m);
s_gl_matrix_3f * gl_matrix_3f_init_matrix_mult
                  (s_gl_matrix_3f *m, const s_gl_matrix_3f *a,
                   const s_gl_matrix_3f *b);

/* Heap-allocation functions, call gl_matrix_3f_delete after use. */
void             gl_matrix_3f_delete (s_gl_matrix_3f *m);
s_gl_matrix_3f * gl_matrix_3f_new_copy (const s_gl_matrix_3f *src);
s_gl_matrix_3f * gl_matrix_3f_new_identity (void);
s_gl_matrix_3f * gl_matrix_3f_new_matrix_mult (const s_gl_matrix_3f *a,
                                               const s_gl_matrix_3f *b);

/* Operators. */
s_gl_matrix_3f * gl_matrix_3f_rotate (s_gl_matrix_3f *m, f32 rad);
s_gl_matrix_3f * gl_matrix_3f_scale (s_gl_matrix_3f *m, f32 x, f32 y);
s_gl_matrix_3f * gl_matrix_3f_translate (s_gl_matrix_3f *m, f32 x,
                                         f32 y);

#endif /* GL_MATRIX_3F_H */
