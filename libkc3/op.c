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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "callable.h"
#include "op.h"

void op_clean (s_op *op)
{
  assert(op);
  p_callable_clean(&op->callable);
}

void op_delete (s_op *op)
{
  assert(op);
  if (op->ref_count <= 0) {
    err_puts("op_delete: invalid reference count");
    assert(! "op_delete: invalid reference count");
    return;
  }
  if (! --op->ref_count) {
    op_clean(op);
    free(op);
  }
}

s_op * op_init (s_op *op)
{
  s_op tmp = {0};
  tmp.ref_count = 1;
  *op = tmp;
  return op;
}

s_op * op_init_copy (s_op *op, const s_op *src)
{
  s_op tmp = {0};
  tmp.sym = src->sym;
  tmp.arity = src->arity;
  tmp.precedence = src->precedence;
  tmp.associativity = src->associativity;
  tmp.callable = callable_new_ref(src->callable);
  tmp.ref_count = 1;
  *op = tmp;
  return op;
}

s_op * op_new (void)
{
  s_op *op;
  op = alloc(sizeof(s_op));
  if (! op)
    return NULL;
  if (! op_init(op)) {
    free(op);
    return NULL;
  }
  return op;
}

s_op * op_new_copy (const s_op *src)
{
  s_op *op;
  op = alloc(sizeof(s_op));
  if (! op)
    return NULL;
  if (! op_init_copy(op, src)) {
    free(op);
    return NULL;
  }
  return op;
}

s_op * op_new_ref (s_op *op)
{
  assert(op);
  if (op->ref_count <= 0) {
    err_puts("op_new_ref: invalid reference count");
    assert(! "op_new_ref: invalid reference count");
    return NULL;
  }
  op->ref_count++;
  return op;
}
