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
#ifndef LIBC3_BINDING_H
#define LIBC3_BINDING_H

#include "types.h"

/* Stack-allocation compatible functions, call binding_clean after
   use. */
void        binding_clean (s_binding *binding);
s_binding * binding_init (s_binding *binding, const s_sym *name,
                          s_binding *next);

/* Heap-allocation functions, call binding_delete* after use. */
s_binding * binding_delete (s_binding *binding);
void        binding_delete_all (s_binding *binding);
s_binding * binding_new (const s_sym *name, s_binding *next);
s_binding * binding_new_copy (const s_binding *src);

/* Observers. */
const s_tag * binding_get (const s_binding *binding, const s_sym *name);
const s_tag * binding_is_bound (const s_binding *binding,
                                const s_sym *name);

/* Operators. */
s_tag * binding_get_w (s_binding *binding, const s_sym *name);
s_binding ** binding_find (s_binding **binding, const s_sym *name);

#endif /* LIBC3_BINDING_H */
