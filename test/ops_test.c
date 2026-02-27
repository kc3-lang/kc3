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
#include "../libkc3/pcallable.h"
#include "../libkc3/struct.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "test.h"

TEST_CASE_PROTOTYPE(ops_init_clean);
TEST_CASE_PROTOTYPE(ops_new_delete);
TEST_CASE_PROTOTYPE(ops_add_tag);
TEST_CASE_PROTOTYPE(ops_get_tag);

void ops_test (void)
{
  TEST_CASE_RUN(ops_init_clean);
  TEST_CASE_RUN(ops_new_delete);
  TEST_CASE_RUN(ops_add_tag);
  TEST_CASE_RUN(ops_get_tag);
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

TEST_CASE(ops_add_tag)
{
  s_ops *ops;
  s_op *op;
  s_tag op_tag = {0};
  test_context("ops_add_tag");
  TEST_ASSERT((ops = ops_new()));
  TEST_EQ(tag_init_pstruct(&op_tag, &g_sym_KC3_Op), &op_tag);
  struct_allocate(op_tag.data.td_pstruct);
  op = op_tag.data.td_pstruct->data;
  op->sym = sym_1("+");
  op->arity = 2;
  op->precedence = 1;
  op->associativity = 1;
  TEST_ASSERT(pcallable_init(&op->pcallable));
  TEST_ASSERT(ops_add_tag(ops, &op_tag));
  TEST_EQ(ops->ht.count, 1);
  tag_clean(&op_tag);
  ops_delete(ops);
  test_context(NULL);
}
TEST_CASE_END(ops_add_tag)

TEST_CASE(ops_get_tag)
{
  s_ops *ops;
  s_op *op;
  s_tag op_tag = {0};
  test_context("ops_get_tag");
  TEST_ASSERT((ops = ops_new()));
  TEST_EQ(tag_init_pstruct(&op_tag, &g_sym_KC3_Op), &op_tag);
  struct_allocate(op_tag.data.td_pstruct);
  op = op_tag.data.td_pstruct->data;
  op->sym = sym_1("+");
  op->arity = 2;
  op->precedence = 3;
  op->associativity = 1;
  TEST_ASSERT(pcallable_init(&op->pcallable));
  TEST_ASSERT(ops_add_tag(ops, &op_tag));
  TEST_EQ(ops->ht.count, 1);
  tag_clean(&op_tag);
  op_tag = (s_tag) {0};
  TEST_ASSERT(ops_get_tag(ops, sym_1("+"), 2, &op_tag));
  TEST_EQ(op_tag.type, TAG_PSTRUCT);
  TEST_ASSERT(op_tag.data.td_pstruct);
  TEST_ASSERT(op_tag.data.td_pstruct->pstruct_type);
  TEST_EQ(op_tag.data.td_pstruct->pstruct_type->module, sym_1("KC3.Op"));
  TEST_ASSERT((op = op_tag.data.td_pstruct->data));
  TEST_EQ(op->sym, sym_1("+"));
  TEST_EQ(op->arity, 2);
  TEST_EQ(op->precedence, 3);
  TEST_EQ(op->associativity, 1);
  tag_clean(&op_tag);
  ops_delete(ops);
  test_context(NULL);
}
TEST_CASE_END(ops_get_tag)
