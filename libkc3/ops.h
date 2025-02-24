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
#ifndef LIBKC3_OPS_H
#define LIBKC3_OPS_H

#include "types.h"

/* Stack-allocation compatible functions, call ops_clean after use. */
void    ops_clean (s_ops *ops);
s_ops * ops_init (s_ops *ops);

/* Observers. */
s_op * ops_get (s_ops *ops, const s_sym *sym, u8 arity);

/* Operators. */
bool ops_add (s_ops *ops, s_op *op);

#endif /* LIBKC3_OPS_H */
