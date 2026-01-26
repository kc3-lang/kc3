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
#ifndef MAT3_H
#define MAT3_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_mat3 * mat3_init_copy (s_mat3 *m, const s_mat3 *src);
s_mat3 * mat3_init_identity (s_mat3 *m);
s_mat3 * mat3_init_matrix_mult (s_mat3 *m, const s_mat3 *a,
                                const s_mat3 *b);

/* Heap-allocation functions, call mat3_delete after use. */
void     mat3_delete (s_mat3 *m);
s_mat3 * mat3_new_copy (const s_mat3 *src);
s_mat3 * mat3_new_identity (void);
s_mat3 * mat3_new_matrix_mult (const s_mat3 *a, const s_mat3 *b);

/* Operators. */
s_mat3 * mat3_rotate (s_mat3 *m, f32 rad);
s_mat3 * mat3_scale (s_mat3 *m, f32 x, f32 y);
s_mat3 * mat3_translate (s_mat3 *m, f32 x, f32 y);

#endif /* MAT3_H */
