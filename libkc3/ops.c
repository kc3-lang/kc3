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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "callable.h"
#include "compare.h"
#include "hash.h"
#include "ht.h"
#include "list.h"
#include "op.h"
#include "ops.h"
#include "pstruct_type.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"

/* Returns true if s_op was added or is already present. */
bool ops_add (s_ops *ops, s_op *op)
{
  s_tag op_tag = {0};
  bool r;
  assert(ops);
  assert(op);
  if (! op ||
      ! op->sym ||
      ! op->arity ||
      op->arity > 2 ||
      op->associativity < 1 ||
      op->associativity > 2 ||
      ! op->pcallable) {
    err_puts("ops_add: invalid op");
    assert(! "ops_add: invalid op");
    return false;
  }
  if (op->special)
    callable_set_special(op->pcallable, true);
  if (! tag_init_pstruct_copy_data(&op_tag, &g_sym_KC3_Op, op)) {
    err_puts("ops_add: tag_init_pstruct_copy_data");
    assert(! "ops_add: tag_init_pstruct_copy_data");
    return false;
  }
  if (false) {
    err_write_1("ops_add: ");
    err_inspect_tag(&op_tag);
    err_write_1("\n");
  }
  r = ht_add(&ops->ht, &op_tag);
  tag_clean(&op_tag);
  return r;
}

/* Returns true if %KC3.Op{} was added or is already present. */
bool ops_add_tag (s_ops *ops, s_tag *op_tag)
{
  s_op *op;
  assert(ops);
  assert(op_tag);
  if (! op_tag ||
      op_tag->type != TAG_PSTRUCT ||
      ! op_tag->data.pstruct ||
      op_tag->data.pstruct->pstruct_type->module != &g_sym_KC3_Op ||
      ! op_tag->data.pstruct->data) {
    err_puts("ops_add_tag: invalid op");
    assert(! "ops_add_tag: invalid op");
    return false;
  }
  op = op_tag->data.pstruct->data;
  if (! op ||
      ! op->sym ||
      ! op->arity ||
      op->arity > 2 ||
      op->associativity < 1 ||
      op->associativity > 2 ||
      ! op->pcallable) {
    err_puts("ops_add_tag: invalid op");
    assert(! "ops_add_tag: invalid op");
    return false;
  }
  if (op->special)
    callable_set_special(op->pcallable, true);
  return ht_add(&ops->ht, op_tag);
}

void ops_clean (s_ops *ops)
{
  assert(ops);
  ht_clean(&ops->ht);
}

s8 ops_compare_tag (const s_tag *a, const s_tag *b)
{
  s_op *op_a = NULL;
  s_op *op_b = NULL;
  s8 r;
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  if (a->type != TAG_PSTRUCT ||
      a->data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
    err_puts("ops_compare_tag: argument a is not a %KC3.Op{}");
    assert(! "ops_compare_tag: argument a is not a %KC3.Op{}");
    abort();
  }
  if (b->type != TAG_PSTRUCT ||
      b->data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
    err_puts("ops_compare_tag: argument b is not a %KC3.Op{}");
    assert(! "ops_compare_tag: argument b is not a %KC3.Op{}");
    abort();
  }
  op_a = a->data.pstruct->data;
  op_b = b->data.pstruct->data;
  r = compare_sym(op_a->sym, op_b->sym);
  if (! r)
    r = compare_u8(op_a->arity, op_b->arity);
  return r;
}

void ops_delete (s_ops *ops)
{
  assert(ops);
  ops_clean(ops);
  free(ops);
}

s_tag * ops_get_tag (s_ops *ops, const s_sym *sym, u8 arity, s_tag *dest)
{
  s_op     op = {0};
  s_struct op_struct = {0};
  s_tag    op_tag = {0};
  s_tag *ptag;
  assert(ops);
  if (! env_global()->loaded || ! sym || ! arity)
    return NULL;
  op_tag.type = TAG_PSTRUCT;
  op_tag.data.pstruct = &op_struct;
  op_struct.data = &op;
  if (! pstruct_type_find(&g_sym_KC3_Op, &op_struct.pstruct_type))
    return NULL;
  if (! op_struct.pstruct_type)
    return NULL;
  op.sym = sym;
  op.arity = arity;
  if (! ht_get(&ops->ht, &op_tag, &ptag))
    return NULL;
  return tag_init_copy(dest, ptag);
}

uw ops_hash_tag (const s_tag *op_tag)
{
  t_hash hash;
  uw h;
  s_op *op;
  assert(op_tag);
  assert(op_tag->type == TAG_PSTRUCT);
  assert(op_tag->data.pstruct);
  assert(op_tag->data.pstruct->pstruct_type);
  assert(op_tag->data.pstruct->pstruct_type->module == &g_sym_KC3_Op);
  assert(op_tag->data.pstruct->data);
  op = op_tag->data.pstruct->data;
  hash_init(&hash);
  hash_update_sym(&hash, op->sym);
  hash_update_u8(&hash, op->arity);
  h = hash_to_uw(&hash);
  hash_clean(&hash);
  return h;
}

s_ops * ops_init (s_ops *ops)
{
  assert(ops);
  *ops = (s_ops) {0};
  if (! ht_init(&ops->ht, &g_sym_KC3_Op, 256))
    return NULL;
  ops->ht.compare = ops_compare_tag;
  ops->ht.hash = ops_hash_tag;
  return ops;
}

s_ops * ops_init_copy (s_ops *ops, s_ops *src)
{
  uw i;
  assert(ops);
  *ops = (s_ops) {0};
  if (! ht_init(&ops->ht, &g_sym_KC3_Op, src->ht.size))
    return NULL;
  ops->ht.compare = ops_compare_tag;
  ops->ht.hash = ops_hash_tag;
  i = 0;
  while (i < ops->ht.size) {
    if (src->ht.items[i])
      ops->ht.items[i] = list_new_copy_all(src->ht.items[i]);
    i++;
  }
  ops->ht.count = src->ht.count;
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

s_ops * ops_new_copy (s_ops *src)
{
  s_ops *ops;
  if (! (ops = alloc(sizeof(s_ops))))
    return NULL;
  if (! ops_init_copy(ops, src)) {
    free(ops);
    return NULL;
  }
  return ops;
}
