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
#ifndef GL_MATRIX_3D_H
#define GL_MATRIX_3D_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_matrix_3d * gl_matrix_3d_init_copy (s_gl_matrix_3d *m,
                                         const s_gl_matrix_3d *src);
s_gl_matrix_3d * gl_matrix_3d_init_identity (s_gl_matrix_3d *m);
s_gl_matrix_3d * gl_matrix_3d_init_matrix_mult
                  (s_gl_matrix_3d *m, const s_gl_matrix_3d *a,
                   const s_gl_matrix_3d *b);

/* Heap-allocation functions, call gl_matrix_3d_delete after use. */
void             gl_matrix_3d_delete (s_gl_matrix_3d *m);
s_gl_matrix_3d * gl_matrix_3d_new_copy (const s_gl_matrix_3d *src);
s_gl_matrix_3d * gl_matrix_3d_new_identity (void);
s_gl_matrix_3d * gl_matrix_3d_new_matrix_mult (const s_gl_matrix_3d *a,
                                               const s_gl_matrix_3d *b);

/* Operators. */
s_gl_matrix_3d * gl_matrix_3d_rotate (s_gl_matrix_3d *m, f64 rad);
s_gl_matrix_3d * gl_matrix_3d_scale (s_gl_matrix_3d *m, f64 x, f64 y);
s_gl_matrix_3d * gl_matrix_3d_translate (s_gl_matrix_3d *m, f64 x,
                                         f64 y);

#endif /* GL_MATRIX_3D_H */
