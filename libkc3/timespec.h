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
 * @file time.h
 * @brief operations on s_timespec
 *
 */
#ifndef LIBKC3_TIMESPEC_H
#define LIBKC3_TIMESPEC_H

#include "types.h"

/* Operators. */
s_timespec * timespec_sub (const s_timespec *a, const s_timespec *b,
                           s_timespec *dest);
f64 *        timespec_to_f64 (const s_timespec *time, f64 *dest);
s_tag *      timespec_to_tag (const s_timespec *time, s_tag *dest);

#endif /* LIBKC3_TIMESPEC_H */
