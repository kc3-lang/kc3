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
#ifndef GL_MATRIX_4D_H
#define GL_MATRIX_4D_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_matrix_4d * gl_matrix_4d_init_copy (s_gl_matrix_4d *m,
                                         const s_gl_matrix_4d *src);
s_gl_matrix_4d * gl_matrix_4d_init_identity (s_gl_matrix_4d *m);
s_gl_matrix_4d * gl_matrix_4d_init_product (s_gl_matrix_4d *m,
                                            const s_gl_matrix_4d *a,
                                            const s_gl_matrix_4d *b);
s_gl_matrix_4d * gl_matrix_4d_init_zero (s_gl_matrix_4d *m);

/* Heap-allocation functions, call gl_matrix_4d_delete after use. */
void             gl_matrix_4d_delete (s_gl_matrix_4d *m);
s_gl_matrix_4d * gl_matrix_4d_new_copy (const s_gl_matrix_4d *src);
s_gl_matrix_4d * gl_matrix_4d_new_identity (void);
s_gl_matrix_4d * gl_matrix_4d_new_matrix_mult (const s_gl_matrix_4d *a,
                                               const s_gl_matrix_4d *b);
s_gl_matrix_4d * gl_matrix_4d_new_zero (void);

/* Operators. */
s_gl_matrix_4d * gl_matrix_4d_ortho (s_gl_matrix_4d *m, f64 x1, f64 x2,
                                     f64 y1, f64 y2, f64 clip_z_near,
                                     f64 clip_z_far);
s_gl_matrix_4d * gl_matrix_4d_perspective (s_gl_matrix_4d *m, f64 fov_y,
                                           f64 aspect_ratio,
                                           f64 clip_z_near,
                                           f64 clip_z_far);
s_gl_matrix_4d * gl_matrix_4d_product (s_gl_matrix_4d *m,
                                       const s_gl_matrix_4d *a);
s_gl_matrix_4d * gl_matrix_4d_rotate_axis (s_gl_matrix_4d *m, f64 rad,
                                           const s_gl_point_3d *axis);
s_gl_matrix_4d * gl_matrix_4d_scale (s_gl_matrix_4d *m, f64 x, f64 y,
                                     f64 z);
s_gl_matrix_4d * gl_matrix_4d_translate (s_gl_matrix_4d *m, f64 x,
                                         f64 y, f64 z);

#endif /* GL_MATRIX_4D_H */
