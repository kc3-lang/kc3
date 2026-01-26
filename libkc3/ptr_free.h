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
#ifndef LIBC3_PTR_FREE_H
#define LIBC3_PTR_FREE_H

#include "types.h"

/* Stack-allocation compatible functions, call ptr_free_clean
   after use. */
void      ptr_free_clean (u_ptr_w *ptr_free);
u_ptr_w * ptr_free_init (u_ptr_w *ptr_free, void *p);
u_ptr_w * ptr_free_init_cast (u_ptr_w *ptr_free,
                              const s_sym * const *type,
                              const s_tag *tag);
u_ptr_w * ptr_free_init_copy (u_ptr_w *ptr_free, const u_ptr_w *src);

/* Heap-allocation functions, call ptr_free_delete after use. */
void      ptr_free_delete (u_ptr_w *ptr_free);
u_ptr_w * ptr_free_new (void *p);

#endif /* LIBC3_PTR_FREE_H */
