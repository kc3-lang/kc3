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
#ifndef VEC3_H
#define VEC3_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_vec3 * vec3_init (s_vec3 *p, f32 x, f32 y, f32 z);
s_vec3 * vec3_init_copy (s_vec3 *p, const s_vec3 *src);
s_vec3 * vec3_init_normalize (s_vec3 *p, const s_vec3 *src);
s_vec3 * vec3_init_zero (s_vec3 *p);

/* Heap-allocation functions, call vec3_delete after use. */
void     vec3_delete (s_vec3 *p);
s_vec3 * vec3_new (f32 x, f32 y, f32 z);
s_vec3 * vec3_new_copy (const s_vec3 *src);
s_vec3 * vec3_new_zero (void);

/* Operators. */
void     vec3_normalize (s_vec3 *p);
s_vec3 * vec3_interpolate (const s_vec3 *a, const s_vec3 *b, f32 t,
                           s_vec3 *dest);

/* Observers. */
f32 vec3_norm (const s_vec3 *p);

#endif /* VEC3_H */
