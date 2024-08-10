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

void list_test (void);
TEST_CASE_PROTOTYPE(list_new_1);

void list_test (void)
{
  TEST_CASE_RUN(list_new_1);
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
