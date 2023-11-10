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
/**
 * @file float.h
 * @brief Floating point numbers operations.
 *
 * Structure to manipulate floating point numbers.
 * C3 floating point numbers are two s_integer :
 *  - num    The numerator.
 *  - div    The divisor, which is always a positive power of 10.
 *
 * All operations are supported in tag_add, tag_mul, tag_neg etc.
 *
 */
#ifndef FLOAT_H
#define FLOAT_H

#include "types.h"

/* Stack-allocation compatible functions */
s_float * float_init (s_float *f);
s_float * float_init_1 (s_float *f, const s8 *s);
s_float * float_init_integer (s_float *f, const s_integer *num,
                              const s_integer *div);
s_float * float_init_w (s_float *f, sw num, uw div);

/* Heap-allocation compatible functions, call float_delete after use */
void      float_delete (s_float *f);
s_float * float_new ();
s_float * float_new_1 (const s8 *s);
s_float * float_new_integer (const s_integer *num,
                             const s_integer *div);
s_float * float_new_w (sw num, uw div);

/* Operators */
s_float * float_add (const s_float *a, const s_float *b, s_float *result);
s_float * float_mul (const s_float *a, const s_float *b, s_float *result);
s_float * float_neg (const s_float *f, s_float *result);
s_float * float_sq (const s_float *f, s_float *result);
s_float * float_sqrt (const s_float *f, s_float *result);
s_float * float_sub (const s_float *a, s_float *b, s_float *result);

#endif /* FLOAT_H */
