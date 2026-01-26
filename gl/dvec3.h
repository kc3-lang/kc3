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
#ifndef DVEC3_H
#define DVEC3_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_dvec3 * dvec3_init (s_dvec3 *p, f64 x, f64 y, f64 z);
s_dvec3 * dvec3_init_copy (s_dvec3 *p, const s_dvec3 *src);
s_dvec3 * dvec3_init_normalize (s_dvec3 *p, const s_dvec3 *src);
s_dvec3 * dvec3_init_product (s_dvec3 *p, const s_dmat4 *m,
                              const s_dvec3 *s);
s_dvec3 * dvec3_init_zero (s_dvec3 *p);

/* Heap-allocation functions, call dvec3_delete after use. */
void      dvec3_delete (s_dvec3 *p);
s_dvec3 * dvec3_new (f64 x, f64 y, f64 z);
s_dvec3 * dvec3_new_copy (const s_dvec3 *src);
s_dvec3 * dvec3_new_product (const s_dmat4 *m,
                             const s_dvec3 *s);
s_dvec3 * dvec3_new_zero (void);

/* Operators. */
s_dvec3 * dvec3_add (const s_dvec3 *a, const s_dvec3 *b, s_dvec3 *dest);
s_dvec3 * dvec3_clamp (const s_dvec3 *a, s_dvec3 *dest);
s_dvec3 * dvec3_cross (const s_dvec3 *a, const s_dvec3 *b,
                       s_dvec3 *dest);
s_dvec3 * dvec3_mul_f64 (const s_dvec3 *a, f64 b, s_dvec3 *dest);
s_dvec3 * dvec3_mul_dvec3 (const s_dvec3 *a, const s_dvec3 *b,
                           s_dvec3 *dest);
s_dvec3 * dvec3_normalize (const s_dvec3 *a, s_dvec3 *dest);
s_dvec3 * dvec3_sub (const s_dvec3 *a, const s_dvec3 *b, s_dvec3 *dest);
s_dvec3 * dvec3_transform (const s_dvec3 *p, const s_dmat4 *matrix,
                           s_dvec3 *dest);

/* Observers. */
f64 dvec3_dot (const s_dvec3 *a, const s_dvec3 *b);
f64 dvec3_norm (const s_dvec3 *p);

#endif /* DVEC3_H */
