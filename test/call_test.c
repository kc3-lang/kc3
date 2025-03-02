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
#include <string.h>
#include "../libkc3/call.h"
#include "../libkc3/str.h"
#include "test.h"

#define CALL_TEST_INIT_1(test)                                         \
  do {                                                                 \
    s_call call;                                                       \
    TEST_EQ(call_init_1(&call, (test)), &call);                        \
    call_clean(&call);                                                 \
    test_ok();                                                         \
  } while (0)

void call_test (void);
TEST_CASE_PROTOTYPE(call_init_1);

void call_test (void)
{
  TEST_CASE_RUN(call_init_1);
}

TEST_CASE(call_init_1)
{
  CALL_TEST_INIT_1("a()");
  CALL_TEST_INIT_1("a(b)");
  CALL_TEST_INIT_1("a(b, c)");
  CALL_TEST_INIT_1("a(b, c, d)");
  CALL_TEST_INIT_1("A.b()");
  CALL_TEST_INIT_1("A.b(c)");
  CALL_TEST_INIT_1("A.b(c, d)");
  CALL_TEST_INIT_1("A.b(c, d, e)");
}
TEST_CASE_END(call_init_1)
