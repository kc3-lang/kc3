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
/**
 * @file time.h
 * @brief operations on s_timespec
 *
 */
#ifndef LIBKC3_TIME_H
#define LIBKC3_TIME_H

#include "types.h"

#ifdef WIN32
#define timegm _mkgmtime
#endif

#define TIME_SECONDS_PER_MINUTE (60)
#define TIME_SECONDS_PER_HOUR   (60 * 60)
#define TIME_SECONDS_PER_DAY    (60 * 60 * 24)
#define TIME_SECONDS_PER_MONTH  (60 * 60 * 24 * 31)
#define TIME_SECONDS_PER_YEAR   (60 * 60 * 24 * 365)

/* Stack-allocation compatible functions. Call time_clean if
   time_init argument allocate was true. */
void     time_clean (s_time *time);
s_time * time_init (s_time *time);
s_time * time_init_add (s_time *time, const s_time *a, const s_time *b);
s_time * time_init_cast (s_time *time, const s_sym * const *type,
                         const s_tag *src);
s_time * time_init_copy (s_time *time, const s_time *src);
s_time * time_init_now (s_time *time);
s_time * time_init_str (s_time *time, const s_str *src);

/* Observers */
s_tag * time_access (s_time *time, s_list *key, s_tag *dest);
s_str * time_diff_to_str (const s_time *time, s_str *dest);
s_str * time_to_str (const s_time *time, s_str *dest);

/* Operators */
s_time * time_allocate (s_time *time);
s_time * time_sub (const s_time *a, const s_time *b,
                   s_time *dest);

s_timespec * timespec_sub (const s_timespec *a, const s_timespec *b,
                           s_timespec *dest);
f64 *        timespec_to_f64 (const s_timespec *time, f64 *dest);
s_tag *      timespec_to_tag (const s_timespec *time, s_tag *dest);

#endif /* LIBKC3_TIME_H */
