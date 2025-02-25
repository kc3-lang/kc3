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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "compare.h"
#include "hash.h"
#include "ht.h"
#include "op.h"
#include "ops.h"
#include "sym.h"

bool ops_add (s_ops *ops, s_op *op)
{
  assert(ops);
  return ht_add(&ops->ht, op);
}

void ops_clean (s_ops *ops)
{
  assert(ops);
  ht_clean(&ops->ht);
}

s8 ops_compare (const s_op *a, const s_op *b)
{
  s8 r;
  r = compare_sym(a->sym, b->sym);
  if (! r)
    r = compare_u8(a->arity, b->arity);
  return r;
}

void ops_delete (s_ops *ops)
{
  assert(ops);
  ops_clean(ops);
  free(ops);
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

uw ops_hash (const s_op *op)
{
  t_hash h;
  uw u;
  hash_init(&h);
  hash_update_sym(&h, &op->sym);
  hash_update_u8(&h, op->arity);
  u = hash_to_uw(&h);
  hash_clean(&h);
  return u;
}

s_ops * ops_init (s_ops *ops)
{
  s_ops tmp = {0};
  assert(ops);
  if (! ht_init(&tmp.ht, &g_sym_KC3_Op, 256))
    return NULL;
  tmp.ht.compare = (s8 (*) (void *, void *)) ops_compare;
  tmp.ht.hash = (uw (*) (void *)) ops_hash;
  tmp.ht.new_ref = (void * (*) (void *)) op_new_ref;
  return ops;
}

s_ops * ops_new (void)
{
  s_ops *ops;
  if (! (ops = alloc(sizeof(s_ops))))
    return NULL;
  if (! ops_init(ops)) {
    free(ops);
    return NULL;
  }
  return ops;
}
