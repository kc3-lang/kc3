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
#include "../libkc3/inspect.h"
#include "test.h"

#define INSPECT_TEST_SYM(test, result)                                 \
  do {                                                                 \
    const s_sym *sym;                                                  \
    s_str str;                                                         \
    assert(test);                                                      \
    assert(result);                                                    \
    test_context("inspect_sym(" # test ") -> " # result);              \
    sym = sym_1(test);                                                 \
    TEST_EQ(inspect_sym(sym, &str), &str);                             \
    TEST_STRNCMP(str.ptr.p, (result), str.size);                       \
    str_clean(&str);                                                   \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(inspect_sym);

void inspect_test (void)
{
  TEST_CASE_RUN(inspect_sym);
}

TEST_CASE(inspect_sym)
{
  INSPECT_TEST_SYM("", ":\"\"");
  INSPECT_TEST_SYM(" ", ":\" \"");
  INSPECT_TEST_SYM("\n", ":\"\\n\"");
  INSPECT_TEST_SYM("\r", ":\"\\r\"");
  INSPECT_TEST_SYM("\t", ":\"\\t\"");
  INSPECT_TEST_SYM("\v", ":\"\\v\"");
  INSPECT_TEST_SYM("\"", ":\"\\\"\"");
  INSPECT_TEST_SYM(".", ":\".\"");
  INSPECT_TEST_SYM("..", ":\"..\"");
  INSPECT_TEST_SYM("...", ":\"...\"");
  INSPECT_TEST_SYM(".. .", ":\".. .\"");
  INSPECT_TEST_SYM("t", ":t");
  INSPECT_TEST_SYM("T", "T");
  INSPECT_TEST_SYM("test", ":test");
  INSPECT_TEST_SYM("Test", "Test");
  INSPECT_TEST_SYM("123", ":123");
  INSPECT_TEST_SYM("test123", ":test123");
  INSPECT_TEST_SYM("Test123", "Test123");
  INSPECT_TEST_SYM("test 123", ":\"test 123\"");
  INSPECT_TEST_SYM("Test 123", ":\"Test 123\"");
  INSPECT_TEST_SYM("test123.test456", ":\"test123.test456\"");
  INSPECT_TEST_SYM("Test123.Test456", "Test123.Test456");
  INSPECT_TEST_SYM("test123(test456)", ":\"test123(test456)\"");
  INSPECT_TEST_SYM("Test123(Test456)", ":\"Test123(Test456)\"");
  INSPECT_TEST_SYM("test123{test456}", ":\"test123{test456}\"");
  INSPECT_TEST_SYM("Test123{Test456}", ":\"Test123{Test456}\"");
  INSPECT_TEST_SYM("É", "É");
  INSPECT_TEST_SYM("Éo", "Éo");
  INSPECT_TEST_SYM("Éoà \n\r\t\v\"",
                   ":\"Éoà \\n\\r\\t\\v\\\"\"");
  INSPECT_TEST_SYM("é", ":é");
  INSPECT_TEST_SYM("éo", ":éo");
  INSPECT_TEST_SYM("éoà \n\r\t\v\"",
                   ":\"éoà \\n\\r\\t\\v\\\"\"");
}
TEST_CASE_END(inspect_sym)
