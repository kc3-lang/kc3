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
#include "op.h"
#include "pcallable.h"
#include "sym.h"

void op_clean (s_op *op)
{
  assert(op);
  pcallable_clean(&op->pcallable);
}

void op_delete (s_op *op)
{
  assert(op);
  op_clean(op);
  free(op);
}

s_op * op_init (s_op *op)
{
  s_op tmp = {0};
  *op = tmp;
  return op;
}

s_op * op_init_copy (s_op *op, s_op *src)
{
  s_op tmp = {0};
  tmp.sym = src->sym;
  tmp.arity = src->arity;
  tmp.special = src->special;
  tmp.precedence = src->precedence;
  tmp.associativity = src->associativity;
  pcallable_init_copy(&tmp.pcallable, &src->pcallable);
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

s_op * op_new_copy (s_op *src)
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

s_op * op_set_callable (s_op *op, s_callable *callable)
{
  if (! pcallable_init_copy(&op->pcallable, &callable))
    return NULL;
  callable_set_special(op->pcallable, op->special);
  return op;
}

s_op * op_set_special (s_op *op, bool special)
{
  op->special = special;
  callable_set_special(op->pcallable, special);
  return op;
}
