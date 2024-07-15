/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
void dvec3_normalize (s_dvec3 *p);
void dvec3_transform (s_dvec3 *p, const s_dmat4 *matrix);

/* Observers. */
f64 dvec3_norm (const s_dvec3 *p);

#endif /* DVEC3_H */
