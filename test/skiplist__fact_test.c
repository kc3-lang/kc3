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
#include <stdlib.h>
#include <string.h>
#include "../libkc3/fact.h"
#include "../libkc3/skiplist__fact.h"
#include "fact_test.h"
#include "test.h"

#define SKIPLIST__FACT_TEST_INSERT(test, expected_length)              \
  do {                                                                 \
    test_context("skiplist_insert__fact(" # test ") -> "               \
                 # expected_length);                                   \
    TEST_ASSERT(skiplist_insert__fact(skiplist, (test)));             \
    TEST_EQ(skiplist->length, (expected_length));                       \
    test_context(NULL);                                                \
  } while (0)

#define SKIPLIST__FACT_TEST_NEW_DELETE(max_height_, spacing)           \
  do {                                                                 \
    s_skiplist__fact *skiplist;                                        \
    TEST_ASSERT((skiplist = skiplist_new__fact(max_height_,            \
                                               spacing)));             \
    TEST_ASSERT(skiplist->head);                                       \
    TEST_ASSERT(skiplist->compare);                                    \
    TEST_EQ(skiplist->length, 0);                                      \
    TEST_EQ(skiplist->max_height, max_height_);                        \
    TEST_EQ(skiplist->head->height, max_height_);                      \
    skiplist_delete__fact(skiplist);                                   \
  } while (0)

#define SKIPLIST__FACT_TEST_REMOVE(test, expected_length)              \
  do {                                                                 \
    test_context("skiplist_remove__fact(" # test ") -> "               \
                 # expected_length);                                   \
    skiplist_remove__fact(skiplist, (test));                          \
    TEST_EQ(skiplist->length, (expected_length));                       \
    test_context(NULL);                                                \
  } while (0)

void skiplist__fact_test (void);
TEST_CASE_PROTOTYPE(skiplist__fact_find);
TEST_CASE_PROTOTYPE(skiplist__fact_insert);
TEST_CASE_PROTOTYPE(skiplist__fact_new_delete);
TEST_CASE_PROTOTYPE(skiplist__fact_remove);

void skiplist__fact_test (void)
{
  TEST_CASE_RUN(skiplist__fact_new_delete);
  TEST_CASE_RUN(skiplist__fact_insert);
  TEST_CASE_RUN(skiplist__fact_remove);
  TEST_CASE_RUN(skiplist__fact_find);
}

TEST_CASE(skiplist__fact_find)
{
  s_fact fact[24];
  const u8 *h;
  const u8 height[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 0};
  sw i;
  char *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "[]",
    "[[], []]",
    "{:a, :b}",
    "{{:a, :b}, {:c, :d}}",
    "{a, b}",
    "{{a, b}, {c, d}}",
    "0",
    "1",
    "10",
    "0x100",
    "0x10000",
    "0x100000000",
    "0x10000000000000000",
    "-1",
    "-10",
    "-0x100",
    "-0x10000",
    "-0x100000000",
    "-0x10000000000000000",
    NULL
  };
  const double *s;
  s_skiplist__fact *skiplist;
  const double spacing[] = {2.0, 2.4, 3.0, 0.0};
  for (h = height; *h; h++) {
    for (s = spacing; *s != 0.0; s++) {
      skiplist = skiplist_new__fact(*h, *s);
      i = 0;
      while (p[i]) {
        fact_test_init_1(fact + i, p[i]);
        skiplist_insert__fact(skiplist, fact + i);
        TEST_ASSERT(skiplist_find__fact(skiplist, fact + i));
        i++;
      }
      i = skiplist->length;
      while (i--) {
        skiplist_remove__fact(skiplist, fact + i);
        TEST_ASSERT(! skiplist_find__fact(skiplist, fact + i));
        fact_test_clean_1(fact + i);
      }
      skiplist_delete__fact(skiplist);
    }
  }
}
TEST_CASE_END(skiplist__fact_find)

TEST_CASE(skiplist__fact_insert)
{
  sw i;
  const u8 *h;
  const u8 height[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 0};
  const double *s;
  const double spacing[] = {2.0, 2.4, 3.0, 0.0};
  char *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "[]",
    "[[], []]",
    "{:a, :b}",
    "{{:a, :b}, {:c, :d}}",
    "{a, b}",
    "{{a, b}, {c, d}}",
    "0",
    "1",
    "10",
    "0x100",
    "0x10000",
    "0x100000000",
    "0x10000000000000000",
    "-1",
    "-10",
    "-0x100",
    "-0x10000",
    "-0x100000000",
    "-0x10000000000000000",
    NULL
  };
  s_fact fact[24];
  s_skiplist__fact *skiplist;
  i = 0;
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    i++;
  }
  for (h = height; *h; h++) {
    for (s = spacing; *s != 0.0; s++) {
      skiplist = skiplist_new__fact(*h, *s);
      i = 0;
      while (p[i]) {
        SKIPLIST__FACT_TEST_INSERT(fact + i, i + 1);
        i++;
      }
      skiplist_delete__fact(skiplist);
    }
  }
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
}
TEST_CASE_END(skiplist__fact_insert)

TEST_CASE(skiplist__fact_new_delete)
{
  const u8 *h;
  const u8  height[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 0};
  const double *s;
  const double  spacing[] = {2.0, 2.4, 3.0, 0.0};
  for (h = height; *h; h++) {
    for (s = spacing; *s != 0.0; s++) {
      SKIPLIST__FACT_TEST_NEW_DELETE(*h, *s);
    }
  }
}
TEST_CASE_END(skiplist__fact_new_delete)

TEST_CASE(skiplist__fact_remove)
{
  s_fact fact[24];
  const u8 *h;
  const u8 height[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 0};
  sw i;
  char *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "[]",
    "[[], []]",
    "{:a, :b}",
    "{{:a, :b}, {:c, :d}}",
    "{a, b}",
    "{{a, b}, {c, d}}",
    "0",
    "1",
    "10",
    "0x100",
    "0x10000",
    "0x100000000",
    "0x10000000000000000",
    "-1",
    "-10",
    "-0x100",
    "-0x10000",
    "-0x100000000",
    "-0x10000000000000000",
    NULL
  };
  const double *s;
  s_skiplist__fact *skiplist;
  const double spacing[] = {2.0, 2.4, 3.0, 0.0};
  for (h = height; *h; h++) {
    for (s = spacing; *s != 0.0; s++) {
      skiplist = skiplist_new__fact(*h, *s);
      i = 0;
      while (p[i]) {
        fact_test_init_1(fact + i, p[i]);
        skiplist_insert__fact(skiplist, fact + i);
        i++;
      }
      i = skiplist->length;
      while (i--) {
        SKIPLIST__FACT_TEST_REMOVE(fact + i, i);
        fact_test_clean_1(fact + i);
      }
      skiplist_delete__fact(skiplist);
    }
  }
}
TEST_CASE_END(skiplist__fact_remove)
