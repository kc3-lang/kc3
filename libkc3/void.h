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
 * @file void.h
 * @brief Generic (void *) operations.
 *
 * Functions to manage (void *) operations.
 */
#ifndef LIBC3_VOID_H
#define LIBC3_VOID_H

#include "types.h"

/* Operators. */
void * void_init_cast (void *v, const s_sym *type, const s_tag *src);

#endif /* LIBC3_VOID_H */
