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
#ifndef LIBKC3_OPS_H
#define LIBKC3_OPS_H

#include "types.h"

/* Stack-allocation compatible functions, call ops_clean after use. */
void    ops_clean (s_ops *ops);
s_ops * ops_init (s_ops *ops);
s_ops * ops_init_copy (s_ops *ops, s_ops *src);

/* Heap-allocation compatible functions, call ops_delete after use. */
void    ops_delete (s_ops *ops);
s_ops * ops_new (void);
s_ops * ops_new_copy (s_ops *src);

/* Observers. */
s8      ops_compare_tag (const s_tag *a, const s_tag *b);
s_op  * ops_get (s_ops *ops, const s_sym *sym, u8 arity, s_op *dest);
s_tag * ops_get_tag (s_ops *ops, const s_sym *sym, u8 arity, s_tag *dest);
uw      ops_hash_tag (const s_tag *op);

/* Operators. */
bool ops_add (s_ops *ops, s_op *op);
bool ops_add_tag (s_ops *ops, s_tag *op_tag);

#endif /* LIBKC3_OPS_H */
