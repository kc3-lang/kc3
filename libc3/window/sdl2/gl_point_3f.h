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
#ifndef GL_POINT_3F_H
#define GL_POINT_3F_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_point_3f * gl_point_3f_init (s_gl_point_3f *p, f32 x, f32 y,
                                  f32 z);
s_gl_point_3f * gl_point_3f_init_copy (s_gl_point_3f *p,
                                       const s_gl_point_3f *src);
s_gl_point_3f * gl_point_3f_init_normalize (s_gl_point_3f *p,
                                            const s_gl_point_3f *src);
s_gl_point_3f * gl_point_3f_init_product (s_gl_point_3f *p,
                                          const s_gl_matrix_4f *m,
                                          const s_gl_point_3f *s);
s_gl_point_3f * gl_point_3f_init_zero (s_gl_point_3f *p);


/* Heap-allocation functions, call gl_point_3f_delete after use. */
void            gl_point_3f_delete (s_gl_point_3f *p);
s_gl_point_3f * gl_point_3f_new (f32 x, f32 y, f32 z);
s_gl_point_3f * gl_point_3f_new_copy (const s_gl_point_3f *src);
s_gl_point_3f * gl_point_3f_new_product (const s_gl_matrix_4f *m,
                                         const s_gl_point_3f *s);
s_gl_point_3f * gl_point_3f_new_zero (void);

/* Operators. */
void gl_point_3f_normalize (s_gl_point_3f *p);
void gl_point_3f_transform (s_gl_point_3f *p,
                            const s_gl_matrix_4f *matrix);

/* Observers. */
f32 gl_point_3f_norm (const s_gl_point_3f *p);

#endif /* GL_POINT_3F_H */
