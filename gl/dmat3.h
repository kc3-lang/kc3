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
#ifndef DMAT3_H
#define DMAT3_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_dmat3 * dmat3_init_copy (s_dmat3 *m, const s_dmat3 *src);
s_dmat3 * dmat3_init_identity (s_dmat3 *m);
s_dmat3 * dmat3_init_matrix_mult (s_dmat3 *m, const s_dmat3 *a,
                                  const s_dmat3 *b);

/* Heap-allocation functions, call dmat3_delete after use. */
void      dmat3_delete (s_dmat3 *m);
s_dmat3 * dmat3_new_copy (const s_dmat3 *src);
s_dmat3 * dmat3_new_identity (void);
s_dmat3 * dmat3_new_matrix_mult (const s_dmat3 *a, const s_dmat3 *b);

/* Operators. */
s_dmat3 * dmat3_rotate (s_dmat3 *m, f64 rad);
s_dmat3 * dmat3_scale (s_dmat3 *m, f64 x, f64 y);
s_dmat3 * dmat3_translate (s_dmat3 *m, f64 x, f64 y);

#endif /* DMAT3_H */
