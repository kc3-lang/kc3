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
#ifndef LIBKC3_CALLABLE_H
#define LIBKC3_CALLABLE_H

#include "types.h"

/* Heap pointer manipulation functions. Call p_callable_clean
   after use. */
void         p_callable_clean (p_callable *callable);
p_callable * p_callable_init (p_callable *callable);
p_callable * p_callable_init_cast (p_callable *callable,
                                   const s_sym * const *type,
                                   const s_tag *tag);
p_callable * p_callable_init_copy (p_callable *callable,
                                   p_callable *src);

/* Heap-allocation functions, call callable_delete after use. */
void         callable_delete (s_callable *callable);
s_callable * callable_new (void);
s_callable * callable_new_copy (s_callable *callable);
s_callable * callable_new_ref (s_callable *callable);

#endif /* LIBKC3_CALLABLE_H */
