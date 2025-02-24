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
#include "assert.h"
#include "ht.h"
#include "ops.h"
#include "sym.h"

bool ops_add (s_ops *ops, s_op *op)
{
  return ht_add(&ops->ht, op);
}

void ops_clean (s_ops *ops)
{
  ht_clean(&ops->ht);
}

s_op * ops_get (s_ops *ops, const s_sym *sym, u8 arity)
{
  s_op op;
  assert(ops);
  assert(sym);
  assert(arity);
  op.sym = sym;
  op.arity = arity;
  return ht_get(&ops->ht, &op);
}

s_ops * ops_init (s_ops *ops)
{
  if (! ht_init(&ops->ht, &g_sym_KC3_Op, 1024))
    return NULL;
  return ops;
}
