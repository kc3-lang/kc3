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
#ifndef LIBKC3_OPS_H
#define LIBKC3_OPS_H

#include "types.h"

/* Stack-allocation compatible functions, call ops_clean after use. */
void    ops_clean (s_ops *ops);
s_ops * ops_init (s_ops *ops);

/* Heap-allocation compatible functions, call ops_delete after use. */
void    ops_delete (s_ops *ops);
s_ops * ops_new (void);

/* Observers. */
s8     ops_compare_op (const s_op *a, const s_op *b);
s_op * ops_get (s_ops *ops, const s_sym *sym, u8 arity);
uw     ops_hash_op (const s_op *op);

/* Operators. */
bool ops_add (s_ops *ops, s_op *op);

#endif /* LIBKC3_OPS_H */
