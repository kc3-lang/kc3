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
 */
#ifndef FLOAT_H
#define FLOAT_H

#include "types.h"

/* Stack-allocation compatible functions */
s_float * float_init (s_float *f);
s_float * float_init_1 (s_float *f, const s8 *s);
s_float * float_init_sw (s_float *f, sw integer, sw bitshift);

/* Heap-allocation compatible functions, call float_delete after use */
void      float_delete (s_float *f);
s_float * float_new (s_float *f);
s_float * float_new_1 (s_float *f, const s8 *s);
s_float * float_new_sw (s_float *f, sw integer, sw bitshift);

/* Modifiers */
s_float * float_add (s_float *a, s_float *b);
s_float * float_mul (s_float *a, s_float *b);
s_float * float_neg (s_float *f);
s_float * float_sq (s_float *f);
s_float * float_sqrt (s_float *f);
s_float * float_sub (s_float *a, s_float *b);

#endif /* FLOAT_H */
