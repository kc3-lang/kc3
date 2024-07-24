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
#ifndef LIBKC3_OPERATOR_H
#define LIBKC3_OPERATOR_H

#include "types.h"

/* Observers */
s8            operator_arity (const s_ident *op);
bool *        operator_find (const s_ident *op, bool *dest);
s_ident *     operator_ident (const s_ident *op, s_ident *dest);
bool *        operator_is_right_associative (const s_ident *op,
                                             bool *dest);
s8            operator_precedence (const s_ident *op);
s_ident *     operator_resolve (const s_ident *ident, u8 arity,
                                s_ident *dest);
const s_sym ** operator_symbol (const s_ident *op, const s_sym **dest);

#endif /* LIBKC3_OPERATOR_H */
