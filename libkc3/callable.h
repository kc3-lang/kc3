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
#ifndef LIBKC3_CALLABLE_H
#define LIBKC3_CALLABLE_H

#include "types.h"

/* Heap-allocation functions, call callable_delete after use. */
void         callable_delete (s_callable *callable);
s_callable * callable_new (void);
s_callable * callable_new_copy (s_callable *callable);
s_callable * callable_new_ref (s_callable *callable);

/* Observers. */
s8           callable_arity (const s_callable *callable);

/* Operators. */
s_callable * callable_set_name (s_callable *callable,
                                const s_ident *name);
s_callable * callable_set_name_if_null (s_callable *callable,
                                        const s_ident *name);
s_callable * callable_set_special (s_callable *callable, bool special);

#endif /* LIBKC3_CALLABLE_H */
