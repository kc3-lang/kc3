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
#ifndef LIBKC3_POINTER_H
#define LIBKC3_POINTER_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_pointer * pointer_init (s_pointer *pointer, const s_sym *pointer_type,
                          const s_sym *target_type, void *p);
s_pointer * pointer_init_cast (s_pointer *pointer,
                               const p_sym *type,
                               s_tag *tag);
s_pointer * pointer_init_copy (s_pointer *pointer,
                               const s_pointer *src);

/* Heap-allocation functions, call pointer_delete after use. */
void        pointer_delete (s_pointer *pointer);
s_pointer * pointer_new (void *p);
s_pointer * pointer_new_copy (const s_pointer *src);

/* Observers. */
s_tag * pointer_deref (const s_pointer *pointer, s_tag *dest);

#endif /* LIBKC3_POINTER_H */
