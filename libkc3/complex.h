/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_COMPLEX_H
#define LIBKC3_COMPLEX_H

#include "types.h"

/* Stack-allocation compatible functions, call complex_clean
   after use. */
void        complex_clean (s_complex *c);
s_complex * complex_init (s_complex *c);
s_complex * complex_init_1 (s_complex *c, const s8 *p);
s_complex * complex_init_cast (s_complex *c, const s_sym * const *type,
                               s_tag *src);
s_complex * complex_init_copy (s_complex *c, s_complex *src);
s_complex * complex_init_f32 (s_complex *c, f32 src);
s_complex * complex_init_f64 (s_complex *c, f64 src);
#if HAVE_F80
s_complex * complex_init_f80 (s_complex *c, f80 src);
#endif
#if HAVE_F128
s_complex * complex_init_f128 (s_complex *c, f128 src);
#endif
s_complex * complex_init_integer (s_complex *c, const s_integer *src);
s_complex * complex_init_ratio (s_complex *c, s_ratio *src);
s_complex * complex_init_s8 (s_complex *c, s8 src);
s_complex * complex_init_s16 (s_complex *c, s16 src);
s_complex * complex_init_s32 (s_complex *c, s32 src);
s_complex * complex_init_s64 (s_complex *c, s64 src);
s_complex * complex_init_sw (s_complex *c, sw src);
s_complex * complex_init_u8 (s_complex *c, u8 src);
s_complex * complex_init_u16 (s_complex *c, u16 src);
s_complex * complex_init_u32 (s_complex *c, u32 src);
s_complex * complex_init_u64 (s_complex *c, u64 src);
s_complex * complex_init_uw (s_complex *c, uw src);

/* Setters */
s_complex * complex_set_double (s_complex *a, double x, double y);

/* Modifiers */
s_complex * complex_add (s_complex *a, s_complex *b,
                         s_complex *dest);
s_complex * complex_div (s_complex *a, s_complex *b,
                         s_complex *dest);
s_complex * complex_mul (s_complex *a, s_complex *b,
                         s_complex *dest);
s_complex * complex_sub (s_complex *a, s_complex *b,
                         s_complex *dest);
f32  complex_to_f32 (s_complex *c);
f64  complex_to_f64 (s_complex *c);
#if HAVE_F80
f80  complex_to_f80 (s_complex *c);
#endif
#if HAVE_F128
f128 complex_to_f128 (s_complex *c);
#endif

/* Heap-allocation functions, call complex_delete after use. */
void        complex_delete (s_complex *c);
s_complex * complex_new (void);
s_complex * complex_new_add (s_complex *a, s_complex *b);
s_complex * complex_new_cast (const s_sym * const *type,
                              s_tag *src);
s_complex * complex_new_div (s_complex *a, s_complex *b);
s_complex * complex_new_mul (s_complex *a, s_complex *b);
s_complex * complex_new_pow (s_complex *a, s_complex *b);
s_complex * complex_new_ref (s_complex *c);
s_complex * complex_new_sub (s_complex *a, s_complex *b);
s_complex * complex_new_copy (s_complex *a);

/* Observers */
bool complex_is_zero (const s_complex *c);

#endif /* LIBKC3_COMPLEX_H */
