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
 * @file void.h
 * @brief Void operations. Be careful :)
 *
 * Structure to manipulate void. Nothing really.
 */
#ifndef LIBC3_VOID_H
#define LIBC3_VOID_H

#include "types.h"

/* Stack-allocation compatible functions */
void * void_init_copy (void *dest, const void *src);

/* Observers */
s_str * void_inspect (void *b, s_str *dest);

#endif /* LIBC3_VOID_H */
