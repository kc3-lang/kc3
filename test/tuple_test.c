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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libc3/tuple.h"
#include "../libc3/str.h"
#include "test.h"

#define TUPLE_TEST_INIT_CLEAN(test)                                    \
  do {                                                                 \
    s_tuple tuple_test;                                                \
    test_context("tuple_init(" # test ")");                            \
    TEST_EQ(tuple_init(&tuple_test, (test)), &tuple_test);             \
    tuple_clean(&tuple_test);                                          \
    test_ok();                                                         \
    test_context(NULL);                                                \
  } while (0)

#define TUPLE_TEST_INIT_1(test)                                        \
  do {                                                                 \
    s_tuple tuple_test;                                                \
    test_context("tuple_init_1(" # test ")");                          \
    TEST_EQ(tuple_init_1(&tuple_test, (test)), &tuple_test);           \
    tuple_clean(&tuple_test);                                          \
    test_context(NULL);                                                \
  } while (0)

#define TUPLE_TEST_INSPECT(test, expected)                             \
  do {                                                                 \
    s_tuple tuple_test;                                                \
    s_str str_result;                                                  \
    test_context("tuple_inspect(" # test ") -> " # expected);          \
    tuple_init_1(&tuple_test, (test));                                 \
    TEST_EQ(tuple_inspect(&tuple_test, &str_result), &str_result);     \
    tuple_clean(&tuple_test);                                          \
    if (g_test_last_ok) {                                              \
      TEST_EQ(str_result.size, strlen(expected));                      \
      if (g_test_last_ok)                                              \
        TEST_STRNCMP(str_result.ptr.p, (expected), str_result.size);   \
      str_clean(&str_result);                                          \
    }                                                                  \
    test_context(NULL);                                                \
  } while (0)

#define TUPLE_TEST_NEW_1(test)                                         \
  do {                                                                 \
    s_tuple *tuple_test;                                               \
    test_context("tuple_new_1(" # test ")");                           \
    TEST_ASSERT((tuple_test = tuple_new_1((test))));                   \
    tuple_delete(tuple_test);                                          \
    test_context(NULL);                                                \
  } while (0)

#define TUPLE_TEST_NEW_DELETE(test)                                    \
  do {                                                                 \
    s_tuple *tuple_test;                                               \
    test_context("tuple_new(" # test ")");                             \
    TEST_ASSERT((tuple_test = tuple_new((test))));                     \
    tuple_delete(tuple_test);                                          \
    test_ok();                                                         \
    test_context(NULL);                                                \
  } while (0)

void tuple_test ();
TEST_CASE_PROTOTYPE(tuple_init_1);
TEST_CASE_PROTOTYPE(tuple_init_clean);
TEST_CASE_PROTOTYPE(tuple_inspect);
TEST_CASE_PROTOTYPE(tuple_new_1);
TEST_CASE_PROTOTYPE(tuple_new_delete);

void tuple_test ()
{
  tuple_test_init_clean();
  tuple_test_new_delete();
  tuple_test_init_1();
  tuple_test_new_1();
  tuple_test_inspect();
}

TEST_CASE(tuple_init_clean)
{
  TUPLE_TEST_INIT_CLEAN(2);
  TUPLE_TEST_INIT_CLEAN(3);
  TUPLE_TEST_INIT_CLEAN(4);
  TUPLE_TEST_INIT_CLEAN(5);
  TUPLE_TEST_INIT_CLEAN(6);
  TUPLE_TEST_INIT_CLEAN(7);
  TUPLE_TEST_INIT_CLEAN(8);
  TUPLE_TEST_INIT_CLEAN(9);
  TUPLE_TEST_INIT_CLEAN(10);
  TUPLE_TEST_INIT_CLEAN(11);
  TUPLE_TEST_INIT_CLEAN(12);
  TUPLE_TEST_INIT_CLEAN(13);
  TUPLE_TEST_INIT_CLEAN(14);
  TUPLE_TEST_INIT_CLEAN(15);
  TUPLE_TEST_INIT_CLEAN(16);
  TUPLE_TEST_INIT_CLEAN(17);
  TUPLE_TEST_INIT_CLEAN(18);
  TUPLE_TEST_INIT_CLEAN(19);
  TUPLE_TEST_INIT_CLEAN(20);
}
TEST_CASE_END(tuple_init_clean)

TEST_CASE(tuple_init_1)
{
  TUPLE_TEST_INIT_1("{:a, :b}");
  TUPLE_TEST_INIT_1("{:a, :b, :c}");
  TUPLE_TEST_INIT_1("{:a, :b, :c, :d}");
  TUPLE_TEST_INIT_1("{{:a, :b}, {:c, :d}}");
  TUPLE_TEST_INIT_1("{{:a, :b}, {:c, :d}, {:e, :f}}");
  TUPLE_TEST_INIT_1("{{:a, :b}, {:c, :d}, {:e, :f}, {:g, :h}}");
  TUPLE_TEST_INIT_1("{{:a, :b}, {:c, :d}, {:e, :f}, {:g, :h}, {:i, :j}}");
}
TEST_CASE_END(tuple_init_1)

TEST_CASE(tuple_inspect)
{
  TUPLE_TEST_INSPECT("{:a, :b}", "{:a, :b}");
  TUPLE_TEST_INSPECT("{{:a, :b}, {:c, :d}}", "{{:a, :b}, {:c, :d}}");
  TUPLE_TEST_INSPECT("{{:a, :b}, {:c, :d}, {:e, :f}}",
                     "{{:a, :b}, {:c, :d}, {:e, :f}}");
}
TEST_CASE_END(tuple_inspect)

TEST_CASE(tuple_new_1)
{
  TUPLE_TEST_NEW_1("{:a, :b}");
  TUPLE_TEST_NEW_1("{:a, :b, :c}");
  TUPLE_TEST_NEW_1("{:a, :b, :c, :d}");
  TUPLE_TEST_NEW_1("{{:a, :b}, {:c, :d}}");
  TUPLE_TEST_NEW_1("{{:a, :b}, {:c, :d}, {:e, :f}}");
  TUPLE_TEST_NEW_1("{{:a, :b}, {:c, :d}, {:e, :f}, {:g, :h}}");
  TUPLE_TEST_NEW_1("{{:a, :b}, {:c, :d}, {:e, :f}, {:g, :h}, {:i, :j}}");
}
TEST_CASE_END(tuple_new_1)

TEST_CASE(tuple_new_delete)
{
  TUPLE_TEST_NEW_DELETE(2);
  TUPLE_TEST_NEW_DELETE(3);
  TUPLE_TEST_NEW_DELETE(4);
  TUPLE_TEST_NEW_DELETE(5);
  TUPLE_TEST_NEW_DELETE(6);
  TUPLE_TEST_NEW_DELETE(7);
  TUPLE_TEST_NEW_DELETE(8);
  TUPLE_TEST_NEW_DELETE(9);
  TUPLE_TEST_NEW_DELETE(10);
  TUPLE_TEST_NEW_DELETE(11);
  TUPLE_TEST_NEW_DELETE(12);
  TUPLE_TEST_NEW_DELETE(13);
  TUPLE_TEST_NEW_DELETE(14);
  TUPLE_TEST_NEW_DELETE(15);
  TUPLE_TEST_NEW_DELETE(16);
  TUPLE_TEST_NEW_DELETE(17);
  TUPLE_TEST_NEW_DELETE(18);
  TUPLE_TEST_NEW_DELETE(19);
  TUPLE_TEST_NEW_DELETE(20);
}
TEST_CASE_END(tuple_new_delete)
