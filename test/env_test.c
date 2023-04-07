/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "../libc3/compare.h"
#include "../libc3/env.h"
#include "../libc3/frame.h"
#include "../libc3/sym.h"
#include "../libc3/tag.h"
#include "test.h"

void env_test ();
void env_test_eval_equal_tag ();
void env_test_init_clean ();

void env_test ()
{
  env_test_init_clean();
  env_test_eval_equal_tag();
}

void env_test_eval_equal_tag ()
{
  s_env env;
  s_frame frame;
  s_tag x;
  s_tag y;
  s_tag z;
  env_init(&env);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = 1");
  TEST_ASSERT(env_eval_equal_tag(&env, tag_init_1(&x, "x"),
                                 tag_init_1(&y, "1"), &z));
  TEST_ASSERT(frame_get(&frame, x.data.ident.sym));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = (1, 2)");
  TEST_ASSERT(env_eval_equal_tag(&env, tag_1(&x, "x"),
                                 tag_1(&y, "(1, 2)"), &z));
  TEST_ASSERT(frame_get(&frame, x.data.ident.sym));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env);
  env.frame = frame_init(&frame, env.frame);
  test_context("() = ()");
  TEST_ASSERT(env_eval_equal_tag(&env, tag_1(&x, "()"),
                                 tag_1(&y, "()"), &z));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = (1, 2)");
  TEST_ASSERT(env_eval_equal_tag(&env, tag_1(&x, "(a, b)"),
                                 tag_1(&y, "(1, 2)"), &z));
  TEST_ASSERT(frame_get(&frame, sym_1("a")));
  TEST_ASSERT(frame_get(&frame, sym_1("b")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = (1, 2)");
  TEST_ASSERT(env_eval_equal_tag(&env, tag_1(&x, "(a | b)"),
                                 tag_1(&y, "(1, 2)"), &z));
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

void env_test_init_clean ()
{
  s_env env;
  env_init(&env);
  test_ok();
  env_clean(&env);
  test_ok();
}
