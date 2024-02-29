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
#ifndef LIBC3_COMPLEX_H
#define LIBC3_COMPLEX_H

#include <stdarg.h>
#include <stdio.h>
#include "types.h"

/* Stack allocation compatible functions */
s_complex * complex_init (s_complex *c);
s_complex * complex_init_1 (s_complex *c, const s8 *p);
s_complex * complex_init_copy (s_complex *a, const s_complex *x);
s_complex * complex_init_f32 (s_complex *a, f32 x, f32 y);
s_complex * complex_init_f64 (s_complex *a, f64 x, f64 y);
void        complex_clean (s_complex *c);

/* Setters */
s_complex * complex_set_double (s_complex *a, double x, double y);

/* Modifiers */
s_complex * complex_add (const s_complex *a, const s_complex *b,
                         s_complex *dest);
s_complex * complex_sub (const s_complex *a, const s_complex *b,
                         s_complex *dest);
s_complex * complex_mul (const s_complex *a, const s_complex *b,
                         s_complex *dest);
s_complex * complex_div (const s_complex *a, const s_complex *b,
                         s_complex *dest);

/* Constructors, call complex_delete after use. */
s_complex * complex_new (void);
s_complex * complex_new_copy (const s_complex *a);

/* Observers */
bool complex_is_zero (const s_complex *c);
f32  complex_to_f32 (const s_complex *c);
f64  complex_to_f64 (const s_complex *c);

#endif /* LIBC3_COMPLEX_H */
