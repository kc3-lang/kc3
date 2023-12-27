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
 * @file clean.h
 * @brief Generic clean-up operations.
 *
 * Functions to manage clean-up code.
 */
#ifndef LIBC3_CLEAN_H
#define LIBC3_CLEAN_H

#include "types.h"

void clean (const s_sym *type, void *data);

#endif /* LIBC3_CLEAN_H */
