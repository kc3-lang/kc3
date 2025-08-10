/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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

/* Stack-allocation compatible functions */
void     call_clean (s_call *call);
s_call * call_init (s_call *call);
s_call * call_init_1 (s_call *call, const char *p);
s_call * call_init_call_cast (s_call *call, const s_sym *type);
s_call * call_init_cast (s_call *call, const s_sym * const *type,
                         s_tag *tag);
s_call * call_init_copy (s_call *call, s_call *src);
s_call * call_init_op (s_call *call);
s_call * call_init_op_unary (s_call *call);

/* Observers. */
sw        call_arity (const s_call *call);
s_ident * call_ident (const s_call *call, s_ident *dest);
p_sym *   call_psym (const s_call *call, p_sym *dest);

/* Modifiers */
bool     call_get (s_call *call);

#endif /* LIBKC3_CALL_H */
