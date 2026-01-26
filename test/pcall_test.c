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
#include "../libkc3/pcall.h"
#include "../libkc3/str.h"
#include "test.h"

#define PCALL_TEST_INIT_1(test)                                        \
  do {                                                                 \
    p_call pcall;                                                      \
    TEST_EQ(pcall_init_1(&pcall, (test)), &pcall);                     \
    pcall_clean(&pcall);                                               \
    test_ok();                                                         \
  } while (0)

void pcall_test (void);
TEST_CASE_PROTOTYPE(pcall_init_1);

void pcall_test (void)
{
  TEST_CASE_RUN(pcall_init_1);
}

TEST_CASE(pcall_init_1)
{
  PCALL_TEST_INIT_1("a()");
  PCALL_TEST_INIT_1("a(b)");
  PCALL_TEST_INIT_1("a(b, c)");
  PCALL_TEST_INIT_1("a(b, c, d)");
  PCALL_TEST_INIT_1("A.b()");
  PCALL_TEST_INIT_1("A.b(c)");
  PCALL_TEST_INIT_1("A.b(c, d)");
  PCALL_TEST_INIT_1("A.b(c, d, e)");
}
TEST_CASE_END(pcall_init_1)
