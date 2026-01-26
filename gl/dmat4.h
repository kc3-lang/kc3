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
#ifndef DMAT4_H
#define DMAT4_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_dmat4 * dmat4_init_copy (s_dmat4 *m, const s_dmat4 *src);
s_dmat4 * dmat4_init_identity (s_dmat4 *m);
s_dmat4 * dmat4_init_product (s_dmat4 *m, const s_dmat4 *a,
                              const s_dmat4 *b);
s_dmat4 * dmat4_init_zero (s_dmat4 *m);

/* Heap-allocation functions, call dmat4_delete after use. */
void      dmat4_delete (s_dmat4 *m);
s_dmat4 * dmat4_new_copy (const s_dmat4 *src);
s_dmat4 * dmat4_new_identity (void);
s_dmat4 * dmat4_new_matrix_mult (const s_dmat4 *a, const s_dmat4 *b);
s_dmat4 * dmat4_new_zero (void);

/* Operators. */
s_dmat4 * dmat4_ortho (s_dmat4 *m, f64 x1, f64 x2, f64 y1, f64 y2,
                       f64 clip_z_near, f64 clip_z_far);
s_dmat4 * dmat4_perspective (s_dmat4 *m, f64 fov_y, f64 aspect_ratio,
                             f64 clip_z_near, f64 clip_z_far);
s_dmat4 * dmat4_product (s_dmat4 *m, const s_dmat4 *a);
s_dmat4 * dmat4_rotate_axis (s_dmat4 *m, f64 rad,
                             const s_dvec3 *axis);
s_dmat4 * dmat4_scale (s_dmat4 *m, f64 x, f64 y, f64 z);
s_dmat4 * dmat4_translate (s_dmat4 *m, f64 x, f64 y, f64 z);

#endif /* DMAT4_H */
