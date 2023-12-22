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
#ifndef LIBC3_RATIO_H
#define LIBC3_RATIO_H

#include <stdarg.h>
#include <stdio.h>
#include "types.h"

void ratio_clean (s_ratio *r);

/* Stack allocation compatible functions */
s_ratio * ratio_init (s_ratio *dest);
s_ratio * ratio_init_integer (s_ratio *r, s_integer *numerator, s_integer *denominator);
s_ratio * ratio_init_zero (s_ratio *r);
s_ratio * ratio_init_1 (s_ratio *r, const char *p);
s_ratio * ratio_init_copy (s_ratio *a, const s_ratio *x);
s_ratio * ratio_init_f32 (s_ratio *a, f32 x);
s_ratio * ratio_init_f64 (s_ratio *a, f64 x);

s_str * ratio_inspect (const s_ratio *src, s_str *dest);

/* Setters */
s_ratio * ratio_set_double (s_ratio *a, double x);

/* Modifiers */
s_ratio * ratio_add (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest);
s_ratio * ratio_sub (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest);
s_ratio * ratio_mul (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest);
s_ratio * ratio_div (const s_ratio *a, const s_ratio *b,
                           s_ratio *dest);

/* Constructors, call ratio_delete after use. */
s_ratio * ratio_new (void);
s_ratio * ratio_new_copy (const s_ratio *a);

/* Observers */
bool ratio_is_zero (const s_ratio *r);
f32  ratio_to_f32 (const s_ratio *r);
f64  ratio_to_f64 (const s_ratio *r);
sw  ratio_to_sw (const s_ratio *r);
s64 ratio_to_s64 (const s_ratio *r);
s32 ratio_to_s32 (const s_ratio *r);
s16 ratio_to_s16 (const s_ratio *r);
s8 ratio_to_s8 (const s_ratio *r);
uw ratio_to_uw (const s_ratio *r);
u64 ratio_to_u64 (const s_ratio *r);
u32 ratio_to_u32 (const s_ratio *r);
u16 ratio_to_u16 (const s_ratio *r);
u8 ratio_to_u8 (const s_ratio *r);


#endif /* LIBC3_RATIO_H */