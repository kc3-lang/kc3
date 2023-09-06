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
 * @file timespec.h
 * @brief operations on struct timespec
 *
 * Struct timespec
 */
#ifndef TIMESPEC_H
#define TIMESPEC_H

#include <time.h>
#include "types.h"

/* Observers */
s_tag * timespec_to_tag (const struct timespec *time, s_tag *dest);

#endif /* TIMESPEC_H */
