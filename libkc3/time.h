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
 * @brief operations on s_time
 *
 */
#ifndef LIBKC3_TIME_H
#define LIBKC3_TIME_H

#include "types.h"

/* Observers */
f64 *   time_to_f64 (const s_time *time, f64 *dest);
s_tag * time_to_tag (const s_time *time, s_tag *dest);

/* Operators */
s_time * time_sub (const s_time *a, const s_time *b, s_time *dest);

#endif /* LIBKC3_TIME_H */
