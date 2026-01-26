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
#ifndef MAT4_H
#define MAT4_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_mat4 * mat4_init_copy (s_mat4 *m, const s_mat4 *src);
s_mat4 * mat4_init_identity (s_mat4 *m);
s_mat4 * mat4_init_product (s_mat4 *m, const s_mat4 *a,
                            const s_mat4 *b);
s_mat4 * mat4_init_scale (s_mat4 *m, f32 x, f32 y, f32 z);
s_mat4 * mat4_init_translate (s_mat4 *m, f32 x, f32 y, f32 z);
s_mat4 * mat4_init_zero (s_mat4 *m);

/* Heap-allocation functions, call mat4_delete after use. */
void     mat4_delete (s_mat4 *m);
s_mat4 * mat4_new_copy (const s_mat4 *src);
s_mat4 * mat4_new_identity (void);
s_mat4 * mat4_new_matrix_mult (const s_mat4 *a, const s_mat4 *b);
s_mat4 * mat4_new_zero (void);

/* Operators. */
s_mat4 * mat4_mult_mat4 (const s_mat4 *a, const s_mat4 *b,
                         s_mat4 *dest);
s_vec3 * mat4_mult_vec3 (const s_mat4 *a, const s_vec3 *b,
                         s_vec3 *dest);
s_mat4 * mat4_ortho (s_mat4 *m, f32 x1, f32 x2, f32 y1, f32 y2,
                     f32 clip_z_near, f32 clip_z_far);
s_mat4 * mat4_perspective (s_mat4 *m, f32 fov_y, f32 aspect_ratio,
                           f32 clip_z_near, f32 clip_z_far);
s_mat4 * mat4_rotate_axis (s_mat4 *m, f32 rad, const s_vec3 *axis);
s_mat4 * mat4_scale (s_mat4 *m, f32 x, f32 y, f32 z);
s_mat4 * mat4_translate (s_mat4 *m, f32 x, f32 y, f32 z);

/* Observers. */
sw mat4_buf_inspect (s_buf *buf, const s_mat4 *matrix);

#endif /* MAT4_H */
