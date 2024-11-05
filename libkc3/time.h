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
/**
 * @file time.h
 * @brief operations on s_timespec
 *
 */
#ifndef LIBKC3_TIME_H
#define LIBKC3_TIME_H

#include "types.h"

/* Stack-allocation compatible functions. Call time_clean if
   time_init argument allocate was true. */
void     time_clean (s_time *time);
s_time * time_init (s_time *time);
s_time * time_init_add (s_time *time, const s_time *a, const s_time *b);
s_time * time_init_copy (s_time *time, const s_time *src);
s_time * time_init_now (s_time *time);
s_time * time_init_str (s_time *time, const s_str *src);

/* Observers */
f64 *   time_to_f64 (const s_timespec *time, f64 *dest);
s_tag * time_to_tag (const s_timespec *time, s_tag *dest);

/* Operators */
s_time * time_allocate (s_time *time);
s_timespec * time_sub (const s_timespec *a, const s_timespec *b, s_timespec *dest);

#endif /* LIBKC3_TIME_H */
