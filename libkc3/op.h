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
#ifndef LIBKC3_OP_H
#define LIBKC3_OP_H

#include "types.h"

/* Stack-allocation compatible functions, call op_clean after use. */
void   op_clean (s_op *op);
s_op * op_init (s_op *op);
s_op * op_init_copy (s_op *op, s_op *src);

/* Heap-allocation compatible functions, call op_delete after use. */
void   op_delete (s_op *op);
s_op * op_new (void);
s_op * op_new_copy (s_op *src);

/* Operators. */
s_op * op_set_special (s_op *op, bool special);
s_op * op_set_callable (s_op *op, s_callable *callable);

#endif /* LIBKC3_OP_H */
