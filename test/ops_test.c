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
#include <assert.h>
#include <string.h>
#include "../libkc3/callable.h"
#include "../libkc3/op.h"
#include "../libkc3/ops.h"
#include "../libkc3/sym.h"
#include "test.h"

TEST_CASE_PROTOTYPE(ops_init_clean);
TEST_CASE_PROTOTYPE(ops_new_delete);
TEST_CASE_PROTOTYPE(ops_add);
TEST_CASE_PROTOTYPE(ops_get);

void ops_test (void)
{
  TEST_CASE_RUN(ops_init_clean);
  TEST_CASE_RUN(ops_new_delete);
  TEST_CASE_RUN(ops_add);
  TEST_CASE_RUN(ops_get);
}

TEST_CASE(ops_init_clean)
{
  s_ops ops;
  test_context("ops_init_clean");
  TEST_EQ(ops_init(&ops), &ops);
  ops_clean(&ops);
  test_context(NULL);
}
TEST_CASE_END(ops_init_clean)

TEST_CASE(ops_new_delete)
{
  s_ops *ops;
  test_context("ops_new_delete");
  TEST_ASSERT((ops = ops_new()));
  ops_delete(ops);
  test_context(NULL);
}
TEST_CASE_END(ops_init_clean)

TEST_CASE(ops_add)
{
  s_ops *ops;
  s_op *op;
  test_context("ops_add");
  TEST_ASSERT((ops = ops_new()));
  TEST_ASSERT((op = op_new()));
  op->sym = sym_1("+");
  op->arity = 2;
  op->precedence = 1;
  op->associativity = 1;
  op->callable = callable_new();
  TEST_ASSERT(ops_add(ops, op));
  op_delete(op);
  ops_delete(ops);
  test_context(NULL);
}
TEST_CASE_END(ops_add)

TEST_CASE(ops_get)
{
  s_ops *ops;
  s_op *op;
  test_context("ops_get");
  TEST_ASSERT((ops = ops_new()));
  TEST_ASSERT((op = op_new()));
  op->sym = sym_1("+");
  op->arity = 2;
  op->precedence = 3;
  op->associativity = 1;
  op->callable = callable_new();
  TEST_ASSERT(ops_add(ops, op));
  op_delete(op);
  TEST_ASSERT((op = ops_get(ops, sym_1("+"), 2)));
  TEST_EQ(op->sym, sym_1("+"));
  TEST_EQ(op->arity, 2);
  TEST_EQ(op->precedence, 3);
  TEST_EQ(op->associativity, 1);
  ops_delete(ops);
  test_context(NULL);
}
TEST_CASE_END(ops_get)
