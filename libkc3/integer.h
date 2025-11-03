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
/**' ,
 * @file integer.h
 * @brief Integer manipulation functions.
 *
 * Structure to manipulate byte strings.
 */
#ifndef LIBKC3_INTEGER_H
#define LIBKC3_INTEGER_H

#include "types.h"

#define MP_IS_ZERO(a) ((a)->used == 0)

/* Stack allocation compatible functions */
s_integer * integer_init (s_integer *i);
s_integer * integer_init_1 (s_integer *i, const char *p);
s_integer * integer_init_cast (s_integer *a, const s_sym * const *type,
                               const s_tag *tag);
s_integer * integer_init_copy (s_integer *a, const s_integer *src);
s_integer * integer_init_f32 (s_integer *a, f32 x);
s_integer * integer_init_f64 (s_integer *a, f64 x);
#if HAVE_F80
s_integer * integer_init_f80 (s_integer *a, f80 x);
#endif
#if HAVE_F128
s_integer * integer_init_f128 (s_integer *a, f128 x);
#endif
s_integer * integer_init_ratio (s_integer *a, const s_ratio *r);
s_integer * integer_init_s8 (s_integer *a, s8 x);
s_integer * integer_init_s16 (s_integer *a, s16 x);
s_integer * integer_init_s32 (s_integer *a, s32 x);
s_integer * integer_init_s64 (s_integer *a, s64 x);
s_integer * integer_init_str (s_integer *a, const s_str *src);
s_integer * integer_init_sw (s_integer *a, sw x);
s_integer * integer_init_u8 (s_integer *a, u8 x);
s_integer * integer_init_u16 (s_integer *a, u16 x);
s_integer * integer_init_u32 (s_integer *a, u32 x);
s_integer * integer_init_u64 (s_integer *a, u64 x);
s_integer * integer_init_uw (s_integer *a, uw x);
s_integer * integer_init_zero (s_integer *i);
void        integer_clean (s_integer *i);

/* Setters */
s_integer * integer_set_f32 (s_integer *a, f32 x);
s_integer * integer_set_f64 (s_integer *a, f64 x);
#if HAVE_F80
s_integer * integer_set_f80 (s_integer *a, f80 x);
#endif
#if HAVE_F128
s_integer * integer_set_f128 (s_integer *a, f128 x);
#endif
s_integer * integer_set_double (s_integer *a, double x);
s_integer * integer_set_ratio (s_integer *a, const s_ratio *r);
s_integer * integer_set_s32 (s_integer *a, s32 x);
s_integer * integer_set_s64 (s_integer *a, s64 x);
s_integer * integer_set_sw (s_integer *a, sw x);
s_integer * integer_set_u32 (s_integer *a, u32 x);
s_integer * integer_set_u64 (s_integer *a, u64 x);
s_integer * integer_set_uw (s_integer *a, uw x);

/* Operators. */
s_integer * integer_abs (const s_integer *a, s_integer *dest);
s_integer * integer_add (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_band (const s_integer *a, const s_integer *b,
                          s_integer *dest);
s_integer * integer_bnot (const s_integer *a, s_integer *dest);
s_integer * integer_bor (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_bxor (const s_integer *a, const s_integer *b,
                          s_integer *dest);
s_integer * integer_div (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_gcd (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_lcm (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_lshift (const s_integer *a, sw b, s_integer *dest);
s_integer * integer_mod (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_mul (const s_integer *a, const s_integer *b,
                         s_integer *dest);
s_integer * integer_neg (const s_integer *a, s_integer *dest);
s_integer * integer_expt_u32 (const s_integer *a, u32 b,
			      s_integer *dest);
s_tag *     integer_sqrt (const s_integer *a, s_tag *dest);
s_integer * integer_sqrt_positive (const s_integer *a, s_integer *dest);
s_integer * integer_sub (const s_integer *a, const s_integer *b,
                         s_integer *dest);

/* Constructors, call integer_delete after use. */
s_integer * integer_new (void);
s_integer * integer_new_copy (const s_integer *a);

/* Observers */
uw      integer_bits (const s_integer *i);
uw      integer_bytes (const s_integer *i);
bool    integer_is_negative (const s_integer *i);
bool    integer_is_positive (const s_integer *i);
bool    integer_is_zero (const s_integer *i);
bool    integer_needs_cast (const s_integer *x);
s_tag * integer_reduce (const s_integer *i, s_tag *dest);
f32     integer_to_f32 (const s_integer *i);
f64     integer_to_f64 (const s_integer *i);
#if HAVE_F80
f80     integer_to_f80 (const s_integer *i);
#endif
#if HAVE_F128
f128    integer_to_f128 (const s_integer *i);
#endif
s8      integer_to_s8 (const s_integer *i);
s16     integer_to_s16 (const s_integer *i);
s32     integer_to_s32 (const s_integer *i);
s64     integer_to_s64 (const s_integer *i);
sw      integer_to_sw (const s_integer *i);
u8      integer_to_u8 (const s_integer *i);
u16     integer_to_u16 (const s_integer *i);
u32     integer_to_u32 (const s_integer *i);
u64     integer_to_u64 (const s_integer *i);
uw      integer_to_uw (const s_integer *i);

#endif /* LIBKC3_INTEGER_H */
