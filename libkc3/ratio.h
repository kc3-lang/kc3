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
#ifndef LIBKC3_RATIO_H
#define LIBKC3_RATIO_H

#include <stdarg.h>
#include <stdio.h>
#include "types.h"

/* Stack-allocation compatible functions, call ratio_clean after use. */
void      ratio_clean (s_ratio *r);
s_ratio * ratio_init (s_ratio *dest);
s_ratio * ratio_init_1 (s_ratio *r, const char *p);
s_ratio * ratio_init_cast (s_ratio *r, p_sym *type, s_tag *src);
s_ratio * ratio_init_copy (s_ratio *r, const s_ratio *src);
s_ratio * ratio_init_f32 (s_ratio *r, f32 src);
s_ratio * ratio_init_f64 (s_ratio *r, f64 src);
#if HAVE_F80
s_ratio * ratio_init_f80 (s_ratio *r, f80 src);
#endif
#if HAVE_F128
s_ratio * ratio_init_f128 (s_ratio *r, f128 src);
#endif
s_ratio * ratio_init_integer (s_ratio *r, const s_integer *src);
s_ratio * ratio_init_ratio (s_ratio *r, const s_ratio *src);
s_ratio * ratio_init_s8 (s_ratio *r, s8 src);
s_ratio * ratio_init_s16 (s_ratio *r, s16 src);
s_ratio * ratio_init_s32 (s_ratio *r, s32 src);
s_ratio * ratio_init_s64 (s_ratio *r, s64 src);
s_ratio * ratio_init_sw (s_ratio *r, sw src);
s_ratio * ratio_init_u8 (s_ratio *r, u8 src);
s_ratio * ratio_init_u16 (s_ratio *r, u16 src);
s_ratio * ratio_init_u32 (s_ratio *r, u32 src);
s_ratio * ratio_init_u64 (s_ratio *r, u64 src);
s_ratio * ratio_init_uw (s_ratio *r, uw src);
s_ratio * ratio_init_zero (s_ratio *r);

/* Heap-allocation functions, call ratio_delete after use. */
void      ratio_delete (s_ratio *r);
s_ratio * ratio_new (void);
s_ratio * ratio_new_copy (const s_ratio *src);

/* Observers. */
bool    ratio_is_negative (const s_ratio *r);
bool    ratio_is_zero (const s_ratio *r);
f32     ratio_to_f32 (const s_ratio *r);
f64     ratio_to_f64 (const s_ratio *r);
#if HAVE_F80
f80     ratio_to_f80 (const s_ratio *r);
#endif
#if HAVE_F128
f128    ratio_to_f128 (const s_ratio *r);
#endif
s8      ratio_to_s8 (const s_ratio *r);
s16     ratio_to_s16 (const s_ratio *r);
s32     ratio_to_s32 (const s_ratio *r);
s64     ratio_to_s64 (const s_ratio *r);
sw      ratio_to_sw (const s_ratio *r);
u8      ratio_to_u8 (const s_ratio *r);
u16     ratio_to_u16 (const s_ratio *r);
u32     ratio_to_u32 (const s_ratio *r);
u64     ratio_to_u64 (const s_ratio *r);
uw      ratio_to_uw (const s_ratio *r);

/* Operators. */
s_ratio * ratio_add (const s_ratio *a, const s_ratio *b,
                     s_ratio *dest);
s_ratio * ratio_div (const s_ratio *a, const s_ratio *b,
                     s_ratio *dest);
s_ratio * ratio_mul (const s_ratio *a, const s_ratio *b,
                     s_ratio *dest);
s_ratio * ratio_neg (const s_ratio *r, s_ratio *dest);
s_ratio * ratio_pow (const s_ratio *a, const s_ratio *b,
                     s_ratio *dest);
s_tag *   ratio_sqrt (const s_ratio *r, s_tag *dest);
s_ratio * ratio_sub (const s_ratio *a, const s_ratio *b,
                     s_ratio *dest);

/* Setters. */
s_ratio * ratio_set_double (s_ratio *a, double x);

s_ratio * ratio_simplify(s_ratio *r, s_ratio *dest);

#endif /* LIBKC3_RATIO_H */
