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
#ifndef OPERATOR_H
#define OPERATOR_H

#include "types.h"

/* Observers */
s_ident * operator_call_ident (const s_ident *op, u8 arity,
                               s_ident *dest);
bool      operator_find (const s_ident *op, u8 arity);
bool      operator_is_right_associative (const s_ident *op);
s8        operator_precedence (const s_ident *op);

#endif /* OPERATOR_H */
