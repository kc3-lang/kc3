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
 * @file bool.h
 * @brief Boolean operations.
 *
 * Structure to manipulate booleans.
 */
#ifndef LIBC3_BOOL_H
#define LIBC3_BOOL_H

#include "types.h"

/* Observers */
bool *  bool_copy (const bool *src, bool *dest);
s_str * bool_inspect (bool *b, s_str *dest);

#endif /* LIBC3_BOOL_H */
