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
#ifndef DVEC2_H
#define DVEC2_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_dvec2 * dvec2_init (s_dvec2 *p, f64 x, f64 y);
s_dvec2 * dvec2_init_copy (s_dvec2 *p, const s_dvec2 *src);
s_dvec2 * dvec2_init_product (s_dvec2 *p, const s_dmat3 *m,
                              const s_dvec2 *s);
s_dvec2 * dvec2_init_zero (s_dvec2 *p);

/* Heap-allocation functions, call dvec2_delete after use. */
void      dvec2_delete (s_dvec2 *p);
s_dvec2 * dvec2_new (f64 x, f64 y);
s_dvec2 * dvec2_new_copy (const s_dvec2 *src);
s_dvec2 * dvec2_new_product (const s_dmat3 *m, const s_dvec2 *s);
s_dvec2 * dvec2_new_zero (void);

#endif /* DVEC2_H */
