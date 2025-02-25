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
#ifndef LIBKC3_OP_H
#define LIBKC3_OP_H

#include "types.h"

/* Stack-allocation compatible functions, call op_clean after use. */
void   op_clean (s_op *op);
s_op * op_init (s_op *op);

#endif /* LIBKC3_OP_H */
