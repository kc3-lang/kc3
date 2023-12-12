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
#ifndef LIBC3_PTR_H
#define LIBC3_PTR_H

#include "types.h"

/* Stack-allocation compatible functions, call ptr_clean after use. */
void    ptr_clean (s_ptr *ptr);
s_ptr * ptr_init (s_ptr *ptr, bool free, const s_sym *type, void *p);

/* Heap-allocation functions, call ptr_delete after use. */
void    ptr_delete (s_ptr *ptr);
s_ptr * ptr_new (bool free, const s_sym *type, void *p);

#endif /* LIBC3_PTR_H */
