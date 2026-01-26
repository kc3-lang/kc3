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
#ifndef VEC2_H
#define VEC2_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_vec2 * vec2_init (s_vec2 *p, f32 x, f32 y);
s_vec2 * vec2_init_copy (s_vec2 *p, const s_vec2 *src);
s_vec2 * vec2_init_product (s_vec2 *p, const s_mat3 *m,
                            const s_vec2 *s);
s_vec2 * vec2_init_zero (s_vec2 *p);

/* Heap-allocation functions, call vec2_delete after use. */
void     vec2_delete (s_vec2 *p);
s_vec2 * vec2_new (f32 x, f32 y);
s_vec2 * vec2_new_copy (const s_vec2 *src);
s_vec2 * vec2_new_product (const s_mat3 *m, const s_vec2 *s);
s_vec2 * vec2_new_zero (void);

#endif /* VEC2_H */
