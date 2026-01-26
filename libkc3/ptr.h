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
#ifndef LIBKC3_PTR_H
#define LIBKC3_PTR_H

#include "types.h"

/* Stack-allocation compatible functions. */
u_ptr_w * ptr_init (u_ptr_w *ptr, void *p);
u_ptr_w * ptr_init_cast (u_ptr_w *ptr,
                         const s_sym * const *type,
                         const s_tag *tag);
u_ptr_w * ptr_init_copy (u_ptr_w *ptr, const u_ptr_w *src);

/* Heap-allocation functions, call ptr_delete after use. */
void      ptr_delete (u_ptr_w *ptr);
u_ptr_w * ptr_new (void *p);
u_ptr_w * ptr_new_copy (const u_ptr_w *src);

#endif /* LIBKC3_PTR_H */
