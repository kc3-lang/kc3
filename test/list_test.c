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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/list.h"
#include "../libkc3/str.h"
#include "test.h"

#define LIST_TEST_NEW_1(test)                                          \
  do {                                                                 \
    s_list *list_test;                                                 \
    test_context("list_new_1(" # test ")");                            \
    TEST_ASSERT((list_test = list_new_1(test)));                       \
    list_delete_all(list_test);                                        \
    test_context(NULL);                                                \
  } while (0)

#define LIST_TEST_NEW_V(...)                                           \
  do {                                                                 \
    s_list *list_test;                                                 \
    test_context("LIST_NEW_V("#__VA_ARGS__")");                        \
    TEST_ASSERT((list_test = LIST_NEW_V(__VA_ARGS__)));                \
    list_delete_all(list_test);                                        \
    test_context(NULL);                                                \
  } while (0)

void list_test (void);
TEST_CASE_PROTOTYPE(list_new_1);
TEST_CASE_PROTOTYPE(list_new_v);

void list_test (void)
{
  TEST_CASE_RUN(list_new_1);
  TEST_CASE_RUN(list_new_v);
}

TEST_CASE(list_new_1)
{
  TEST_ASSERT(! list_new_1("[]"));
  LIST_TEST_NEW_1("[[] | []]");
  LIST_TEST_NEW_1("[[], [] | []]");
  LIST_TEST_NEW_1("[[], [], [] | []]");
  LIST_TEST_NEW_1("[A]");
  LIST_TEST_NEW_1("[A | B]");
  LIST_TEST_NEW_1("[A, B]");
  LIST_TEST_NEW_1("[A, B | C]");
  LIST_TEST_NEW_1("[A, B, C]");
  LIST_TEST_NEW_1("[A, B, C | D]");
}
TEST_CASE_END(list_new_1)

TEST_CASE(list_new_v)
{
  TEST_ASSERT(! LIST_NEW_V());
  LIST_TEST_NEW_V([] | []);
  LIST_TEST_NEW_V([], [] | []);
  LIST_TEST_NEW_V([], [], [] | []);
  LIST_TEST_NEW_V(A);
  LIST_TEST_NEW_V(A | B);
  LIST_TEST_NEW_V(A, B);
  LIST_TEST_NEW_V(A, B | C);
  LIST_TEST_NEW_V(A, B, C);
  LIST_TEST_NEW_V(A, B, C | D);
}
TEST_CASE_END(list_new_v)
