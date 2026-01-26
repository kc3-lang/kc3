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
#include <string.h>
#include "../libkc3/fn.h"
#include "test.h"

#define FN_TEST_INIT_1(test)                                           \
  do {                                                                 \
    TEST_EQ(fn_init_1(&f, (test)), &f);                                \
    fn_clean(&f);                                                      \
  } while (0)

TEST_CASE_PROTOTYPE(fn_init_1);

void fn_test (void);

void fn_test (void)
{
  TEST_CASE_RUN(fn_init_1);
}

TEST_CASE(fn_init_1)
{
  s_fn f;
  FN_TEST_INIT_1("fn (x) { x }");
  FN_TEST_INIT_1("fn (x) { x * 2 }");
  FN_TEST_INIT_1("fn (x, y) { x + y }");
  FN_TEST_INIT_1("fn { (x) { x } (x, y) { x + y } }");
  FN_TEST_INIT_1("fn { ([]) { :error } ([x | _y]) { x } (_) { :error } }");
}
TEST_CASE_END(fn_init_1)
