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
#include <string.h>
#include "../libc3/call.h"
#include "../libc3/str.h"
#include "test.h"

#define CALL_TEST_INIT_1(test)                                         \
  do {                                                                 \
    s_call call;                                                       \
    TEST_EQ(call_init_1(&call, (test)), &call);                        \
    call_clean(&call);                                                 \
    test_ok();                                                         \
  } while (0)

#define CALL_TEST_INSPECT(test)                                        \
  do {                                                                 \
    s_call call;                                                       \
    s_str result;                                                      \
    call_init_1(&call, (test));                                        \
    TEST_EQ(call_inspect(&call, &result), &result);                    \
    TEST_EQ(result.size, strlen(test));                                \
    TEST_STRNCMP(result.ptr.p, (test), result.size);                   \
    call_clean(&call);						       \
    str_clean(&result);						       \
  } while (0)

void call_test ();
void call_test_init_1 ();
void call_test_inspect ();

void call_test ()
{
  call_test_init_1();
  call_test_inspect();
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

TEST_CASE(call_inspect)
{
  CALL_TEST_INSPECT("a()");
  CALL_TEST_INSPECT("a(b)");
  CALL_TEST_INSPECT("a(b, c)");
  CALL_TEST_INSPECT("a(b, c, d)");
  CALL_TEST_INSPECT("A.b()");
  CALL_TEST_INSPECT("A.b(c)");
  CALL_TEST_INSPECT("A.b(c, d)");
  CALL_TEST_INSPECT("A.b(c, d, e)");
}
TEST_CASE_END(call_inspect)
