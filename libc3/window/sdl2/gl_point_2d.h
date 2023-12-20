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
#ifndef GL_POINT_2D_H
#define GL_POINT_2D_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_gl_point_2d * gl_point_2d_init (s_gl_point_2d *p, f64 x, f64 y);
s_gl_point_2d * gl_point_2d_init_copy (s_gl_point_2d *p,
                                       const s_gl_point_2d *src);
s_gl_point_2d * gl_point_2d_init_product (s_gl_point_2d *p,
                                          const s_gl_matrix_3d *m,
                                          const s_gl_point_2d *s);
s_gl_point_2d * gl_point_2d_init_zero (s_gl_point_2d *p);


/* Heap-allocation functions, call gl_point_2d_delete after use. */
void            gl_point_2d_delete (s_gl_point_2d *p);
s_gl_point_2d * gl_point_2d_new (f64 x, f64 y);
s_gl_point_2d * gl_point_2d_new_copy (const s_gl_point_2d *src);
s_gl_point_2d * gl_point_2d_new_product (const s_gl_matrix_3d *m,
                                         const s_gl_point_2d *s);
s_gl_point_2d * gl_point_2d_new_zero (void);

#endif /* GL_POINT_2D_H */
