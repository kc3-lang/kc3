/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libc3/list.h"
#include "../libc3/str.h"
#include "test.h"

#define LIST_TEST_1(test)                                              \
  do {                                                                 \
    s_list *list_test;                                                 \
    test_context("list_1(" # test ")");                                \
    TEST_ASSERT((list_test = list_1(test)));                           \
    list_delete_all(list_test);                                            \
    test_context(NULL);                                                \
  } while (0)

#define LIST_TEST_INSPECT(test, expected)                              \
  do {                                                                 \
    s_list *list_test;                                                 \
    s_str str_result;                                                  \
    test_context("list_inspect(" # test ") -> " # expected);           \
    list_test = list_1(test);                                          \
    TEST_EQ(list_inspect(list_test, &str_result), &str_result);        \
    TEST_STRNCMP(str_result.ptr.p, (expected), str_result.size);       \
    str_clean(&str_result);                                            \
    list_delete_all(list_test);                                            \
    test_context(NULL);                                                \
  } while (0)

void list_test ();
void list_test_1 ();
void list_test_inspect ();

void list_test ()
{
  list_test_1();
  list_test_inspect();
}

void list_test_1 ()
{
  TEST_ASSERT(! list_1("[]"));
  LIST_TEST_1("[[] | []]");
  LIST_TEST_1("[[], [] | []]");
  LIST_TEST_1("[[], [], [] | []]");
  LIST_TEST_1("[A]");
  LIST_TEST_1("[A | B]");
  LIST_TEST_1("[A, B]");
  LIST_TEST_1("[A, B | C]");
  LIST_TEST_1("[A, B, C]");
  LIST_TEST_1("[A, B, C | D]");
}

void list_test_inspect ()
{
  LIST_TEST_INSPECT("[]", "[]");
  LIST_TEST_INSPECT("[[] | []]", "[[]]");
  LIST_TEST_INSPECT("[[], [] | []]", "[[], []]");
}
