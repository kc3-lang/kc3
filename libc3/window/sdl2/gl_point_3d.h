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
#ifndef GL_POINT_3D_H
#define GL_POINT_3D_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_point_3d * gl_point_3d_init (s_gl_point_3d *p, f64 x, f64 y,
                                  f64 z);
s_gl_point_3d * gl_point_3d_init_copy (s_gl_point_3d *p,
                                       const s_gl_point_3d *src);
s_gl_point_3d * gl_point_3d_init_normalize (s_gl_point_3d *p,
                                            const s_gl_point_3d *src);
s_gl_point_3d * gl_point_3d_init_product (s_gl_point_3d *p,
                                          const s_gl_matrix_4d *m,
                                          const s_gl_point_3d *s);
s_gl_point_3d * gl_point_3d_init_zero (s_gl_point_3d *p);


/* Heap-allocation functions, call gl_point_3d_delete after use. */
void            gl_point_3d_delete (s_gl_point_3d *p);
s_gl_point_3d * gl_point_3d_new (f64 x, f64 y, f64 z);
s_gl_point_3d * gl_point_3d_new_copy (const s_gl_point_3d *src);
s_gl_point_3d * gl_point_3d_new_product (const s_gl_matrix_4d *m,
                                         const s_gl_point_3d *s);
s_gl_point_3d * gl_point_3d_new_zero (void);

/* Operators. */
void gl_point_3d_normalize (s_gl_point_3d *p);
void gl_point_3d_transform (s_gl_point_3d *p,
                            const s_gl_matrix_4d *matrix);

/* Observers. */
f64 gl_point_3d_norm (const s_gl_point_3d *p);

#endif /* GL_POINT_3D_H */
