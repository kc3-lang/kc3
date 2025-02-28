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
#include "../libkc3/call.h"
#include "../libkc3/compare.h"
#include "../libkc3/env.h"
#include "../libkc3/frame.h"
#include "../libkc3/list.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "test.h"

void env_test (void);
TEST_CASE_PROTOTYPE(env_eval_call);
TEST_CASE_PROTOTYPE(env_eval_equal_tag);
TEST_CASE_PROTOTYPE(env_eval_tag);
TEST_CASE_PROTOTYPE(env_init_clean);
TEST_CASE_PROTOTYPE(env_module_load);

void env_test (void)
{
  TEST_CASE_RUN(env_init_clean);
  TEST_CASE_RUN(env_eval_equal_tag);
  TEST_CASE_RUN(env_eval_call);
  TEST_CASE_RUN(env_eval_tag);
  TEST_CASE_RUN(env_module_load);
}

TEST_CASE(env_eval_call)
{
  s_env env;
  s_call call;
  s_tag result;
  s_tag expected;
  env_init(&env, 0, NULL);
  test_context("env_eval_call(1 + 2) -> 3");
  call_init(&call);
  call.ident.module = sym_1("KC3");
  call.ident.sym = sym_1("op_add");
  call.arguments = list_new_1("[1, 2]");
  tag_init_u8(&expected, 3);
  TEST_ASSERT(env_eval_call(&env, &call, &result));
  TEST_EQ(compare_tag(&result, &expected), 0);
  call_clean(&call);
  tag_clean(&result);
  tag_clean(&expected);
  env_clean(&env);
  test_context(NULL);
}
TEST_CASE_END(env_eval_call)

TEST_CASE(env_eval_equal_tag)
{
  s_env env;
  s_frame frame;
  s_tag x;
  s_tag y;
  s_tag z;
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame, NULL);
  test_context("x = 1");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_init_1(&x, "x"),
                                 tag_init_1(&y, "1"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, x.data.ident.sym));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame, NULL);
  test_context("x = (1, 2]");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_init_1(&x, "x"),
                                 tag_init_1(&y, "[1, 2]"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, sym_1("x")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame, NULL);
  test_context("[] = []");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_1(&x, "[]"),
                                 tag_1(&y, "[]"),
                                 &z));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame, NULL);
  test_context("[a, b] = [1, 2]");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_1(&x, "[a, b]"),
                                 tag_1(&y, "[1, 2]"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, sym_1("a")));
  TEST_ASSERT(frame_get(&frame, sym_1("b")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame, NULL);
  test_context("x = [1, 2]");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_1(&x, "[a | b]"),
                                 tag_1(&y, "[1, 2]"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, sym_1("a")));
  TEST_ASSERT(frame_get(&frame, sym_1("b")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  tag_clean(&x);
  tag_clean(&y);
  test_context(NULL);
}
TEST_CASE_END(env_eval_equal_tag)

TEST_CASE(env_eval_tag)
{
  s_env env;
  s_tag x;
  s_tag y;
  s_tag expected;
  env_init(&env, 0, NULL);
  test_context("env_eval_tag(1 + 2) -> 3");
  TEST_EQ(tag_init_1(&x, "1 + 2"), &x);
  TEST_EQ(tag_init_1(&expected, "3"), &expected);
  TEST_ASSERT(env_eval_tag(&env, &x, &y));
  TEST_EQ(compare_tag(&y, &expected), 0);
  tag_clean(&x);
  tag_clean(&y);
  tag_clean(&expected);
  env_clean(&env);
  test_context(NULL);
}
TEST_CASE_END(env_eval_tag)

TEST_CASE(env_init_clean)
{
  s_env env;
  env_init(&env, 0, NULL);
  env_clean(&env);
}
TEST_CASE_END(env_init_clean)

TEST_CASE(env_module_load)
{
  s_env env;
  env_init(&env, 0, NULL);
  TEST_ASSERT(env_module_load(&env, sym_1("KC3.Operator")));
  env_clean(&env);
}
TEST_CASE_END(env_module_load)
