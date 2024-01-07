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
#ifndef GL_MATRIX_4F_H
#define GL_MATRIX_4F_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_matrix_4f * gl_matrix_4f_init_copy (s_gl_matrix_4f *m,
                                         const s_gl_matrix_4f *src);
s_gl_matrix_4f * gl_matrix_4f_init_identity (s_gl_matrix_4f *m);
s_gl_matrix_4f * gl_matrix_4f_init_product (s_gl_matrix_4f *m,
                                            const s_gl_matrix_4f *a,
                                            const s_gl_matrix_4f *b);
s_gl_matrix_4f * gl_matrix_4f_init_zero (s_gl_matrix_4f *m);

/* Heap-allocation functions, call gl_matrix_4f_delete after use. */
void             gl_matrix_4f_delete (s_gl_matrix_4f *m);
s_gl_matrix_4f * gl_matrix_4f_new_copy (const s_gl_matrix_4f *src);
s_gl_matrix_4f * gl_matrix_4f_new_identity (void);
s_gl_matrix_4f * gl_matrix_4f_new_matrix_mult (const s_gl_matrix_4f *a,
                                               const s_gl_matrix_4f *b);
s_gl_matrix_4f * gl_matrix_4f_new_zero (void);

/* Operators. */
s_gl_matrix_4f * gl_matrix_4f_ortho (s_gl_matrix_4f *m, f32 x1, f32 x2,
                                     f32 y1, f32 y2, f32 clip_z_near,
                                     f32 clip_z_far);
s_gl_matrix_4f * gl_matrix_4f_perspective (s_gl_matrix_4f *m, f32 fov_y,
                                           f32 aspect_ratio,
                                           f32 clip_z_near,
                                           f32 clip_z_far);
s_gl_matrix_4f * gl_matrix_4f_product (s_gl_matrix_4f *m,
                                       const s_gl_matrix_4f *a);
s_gl_matrix_4f * gl_matrix_4f_rotate_axis (s_gl_matrix_4f *m, f32 rad,
                                           const s_gl_point_3f *axis);
s_gl_matrix_4f * gl_matrix_4f_scale (s_gl_matrix_4f *m, f32 x, f32 y,
                                     f32 z);
s_gl_matrix_4f * gl_matrix_4f_translate (s_gl_matrix_4f *m, f32 x,
                                         f32 y, f32 z);

#endif /* GL_MATRIX_4F_H */
