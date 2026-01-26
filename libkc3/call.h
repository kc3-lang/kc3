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
#ifndef LIBKC3_CALL_H
#define LIBKC3_CALL_H

#include "types.h"

/* Stack-allocation compatible functions, call call_clean after use. */
void     call_clean (s_call *call);
s_call * call_init (s_call *call);
s_call * call_init_call_cast (s_call *call, const s_sym *type);
s_call * call_init_copy (s_call *call, s_call *src);
s_call * call_init_op (s_call *call);
s_call * call_init_op_unary (s_call *call);

/* Heap-allocation functions, call call_delete after use. */
void     call_delete (s_call *call);
s_call * call_new (void);
s_call * call_new_call_cast (const s_sym *type);
s_call * call_new_cast (const s_sym * const *type,
                        s_tag *tag);
s_call * call_new_copy (s_call *src);
s_call * call_new_op (void);
s_call * call_new_op_unary (void);
s_call * call_new_ref (s_call *src);

/* Observers. */
sw        call_arity (const s_call *call);

/* Modifiers */
bool     call_get (s_call *call);

#endif /* LIBKC3_CALL_H */
