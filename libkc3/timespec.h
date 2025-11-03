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
/**
 * @file timespec.h
 * @brief operations on s_timespec
 *
 */
#ifndef LIBKC3_TIMESPEC_H
#define LIBKC3_TIMESPEC_H

#include "types.h"

#define DEF_TIMESPEC_INIT(type)                                       \
  s_timespec * timespec_init_ ## type (s_timespec *time, type src)    \

/* Stack-allocation compatible functions. */
s_timespec * timespec_init_cast (s_timespec *time, p_sym *type,
                                 s_tag *src);
DEF_TIMESPEC_INIT(f32);
DEF_TIMESPEC_INIT(f64);
#if HAVE_F80
DEF_TIMESPEC_INIT(f80);
#endif
#if HAVE_FLOAT128
DEF_TIMESPEC_INIT(f128);
#endif
DEF_TIMESPEC_INIT(u8);
DEF_TIMESPEC_INIT(u16);
DEF_TIMESPEC_INIT(u32);
DEF_TIMESPEC_INIT(u64);
DEF_TIMESPEC_INIT(uw);

/* Operators. */
s_timespec * timespec_sub (const s_timespec *a, const s_timespec *b,
                           s_timespec *dest);
f64 *        timespec_to_f64 (const s_timespec *time, f64 *dest);
s_tag *      timespec_to_tag (const s_timespec *time, s_tag *dest);

#endif /* LIBKC3_TIMESPEC_H */
