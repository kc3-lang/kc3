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
#include "../libkc3/set_cursor__fact.h"
#include "../libkc3/set__fact.h"
#include "fact_test.h"
#include "test.h"

#define SET__FACT_TEST_ADD(test, expected_count)                       \
  do {                                                                 \
    test_context("set_add__fact(" # test ") -> " # expected_count);    \
    TEST_ASSERT(set_add__fact(&set, (test)));                          \
    TEST_EQ(set.count, (expected_count));                              \
    test_context(NULL);                                                \
  } while (0)

#define SET__FACT_TEST_CURSOR(count)                                   \
  do {                                                                 \
    sw j = 0;                                                          \
    sw k = (count);                                                    \
    s_set_cursor__fact cursor;                                         \
    test_context("set_cursor");                                        \
    set_cursor_init__fact(&set, &cursor);                              \
    while (j < k) {                                                    \
      TEST_ASSERT(set_cursor_next__fact(&cursor));                     \
      j++;                                                             \
    }                                                                  \
    TEST_ASSERT(! set_cursor_next__fact(&cursor));                     \
    test_context(NULL);                                                \
  } while (0)

#define SET__FACT_TEST_INIT_CLEAN(max_)                                \
  do {                                                                 \
    s_set__fact set;                                                   \
    test_context("set_init__fact(" # max_ ")");                        \
    TEST_EQ(set_init__fact(&set, max_), &set);                         \
    TEST_EQ(set.collisions, 0);                                        \
    TEST_EQ(set.count, 0);                                             \
    TEST_EQ(set.max, max_);                                            \
    set_clean__fact(&set);					       \
    test_context(NULL);                                                \
  } while (0)

#define SET__FACT_TEST_REMOVE(test, expected_count)                    \
  do {                                                                 \
    test_context("set_remove__fact(" # test ") -> " # expected_count); \
    set_remove__fact(&set, (test));                                    \
    TEST_EQ(set.count, (expected_count));                              \
    test_context(NULL);                                                \
  } while (0)

#define SET__FACT_TEST_RESIZE(max_)                                    \
  do {                                                                 \
    test_context("set_resize__fact(" # max_ ")");                      \
    set_resize__fact(&set, (max_));                                    \
    i = set.count;                                                     \
    while (i--) {                                                      \
      TEST_ASSERT(set_get__fact(&set, fact + i));                      \
    }                                                                  \
    test_context(NULL);                                                \
  } while (0)

void set__fact_test (void);
TEST_CASE_PROTOTYPE(set__fact_add);
TEST_CASE_PROTOTYPE(set__fact_cursor);
TEST_CASE_PROTOTYPE(set__fact_init_clean);
TEST_CASE_PROTOTYPE(set__fact_remove);
TEST_CASE_PROTOTYPE(set__fact_resize);

void set__fact_test (void)
{
  TEST_CASE_RUN(set__fact_init_clean);
  TEST_CASE_RUN(set__fact_add);
  TEST_CASE_RUN(set__fact_remove);
  TEST_CASE_RUN(set__fact_resize);
  TEST_CASE_RUN(set__fact_cursor);
}

TEST_CASE(set__fact_add)
{
  s_fact fact[24];
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
  s_set__fact set;
  set_init__fact(&set, 64);
  i = 0;
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    set_add__fact(&set, fact + i);
    SET__FACT_TEST_ADD(fact + i, i + 1);
    SET__FACT_TEST_ADD(fact + i, i + 1);
    i++;
  }
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  set_clean__fact(&set);
}
TEST_CASE_END(set__fact_add)

TEST_CASE(set__fact_cursor)
{
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
  s_fact fact[24];
  s_set__fact set;
  set_init__fact(&set, 16);
  i = 0;
  SET__FACT_TEST_CURSOR(0);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    set_add__fact(&set, fact + i);
    i++;
    SET__FACT_TEST_CURSOR(i);
  }
  i = set.count;
  while (i--) {
    set_remove__fact(&set, fact + i);
    SET__FACT_TEST_CURSOR(i);
    fact_test_clean_1(fact + i);
  }
  set_clean__fact(&set);
}
TEST_CASE_END(set__fact_cursor)

TEST_CASE(set__fact_init_clean)
{
  SET__FACT_TEST_INIT_CLEAN(1);
  SET__FACT_TEST_INIT_CLEAN(8);
  SET__FACT_TEST_INIT_CLEAN(10);
  SET__FACT_TEST_INIT_CLEAN(16);
  SET__FACT_TEST_INIT_CLEAN(20);
  SET__FACT_TEST_INIT_CLEAN(32);
  SET__FACT_TEST_INIT_CLEAN(40);
  SET__FACT_TEST_INIT_CLEAN(50);
  SET__FACT_TEST_INIT_CLEAN(64);
  SET__FACT_TEST_INIT_CLEAN(80);
  SET__FACT_TEST_INIT_CLEAN(90);
  SET__FACT_TEST_INIT_CLEAN(100);
  SET__FACT_TEST_INIT_CLEAN(128);
  SET__FACT_TEST_INIT_CLEAN(200);
  SET__FACT_TEST_INIT_CLEAN(256);
  SET__FACT_TEST_INIT_CLEAN(300);
  SET__FACT_TEST_INIT_CLEAN(500);
  SET__FACT_TEST_INIT_CLEAN(512);
  SET__FACT_TEST_INIT_CLEAN(600);
  SET__FACT_TEST_INIT_CLEAN(700);
  SET__FACT_TEST_INIT_CLEAN(800);
  SET__FACT_TEST_INIT_CLEAN(900);
  SET__FACT_TEST_INIT_CLEAN(1000);
  SET__FACT_TEST_INIT_CLEAN(1024);
  SET__FACT_TEST_INIT_CLEAN(10000);
  SET__FACT_TEST_INIT_CLEAN(16 * 1024);
  SET__FACT_TEST_INIT_CLEAN(100000);
  SET__FACT_TEST_INIT_CLEAN(128 * 1024);
  SET__FACT_TEST_INIT_CLEAN(1000000);
  SET__FACT_TEST_INIT_CLEAN(1024 * 1024);
}
TEST_CASE_END(set__fact_init_clean)

TEST_CASE(set__fact_remove)
{
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
  s_fact fact[24];
  s_set__fact set;
  set_init__fact(&set, 16);
  i = 0;
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    set_add__fact(&set, fact + i);
    i++;
  }
  i = set.count;
  while (i--) {
    SET__FACT_TEST_REMOVE(fact + i, i);
    fact_test_clean_1(fact + i);
  }
  set_clean__fact(&set);
}
TEST_CASE_END(set__fact_remove)

TEST_CASE(set__fact_resize)
{
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
  s_fact fact[24];
  s_set__fact set;
  set_init__fact(&set, 16);
  i = 0;
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    set_add__fact(&set, fact + i);
    i++;
  }
  SET__FACT_TEST_RESIZE(1);
  TEST_EQ(set.collisions, 22);
  SET__FACT_TEST_RESIZE(1);
  SET__FACT_TEST_RESIZE(2);
  SET__FACT_TEST_RESIZE(2);
  SET__FACT_TEST_RESIZE(4);
  SET__FACT_TEST_RESIZE(4);
  SET__FACT_TEST_RESIZE(8);
  SET__FACT_TEST_RESIZE(8);
  SET__FACT_TEST_RESIZE(16);
  SET__FACT_TEST_RESIZE(16);
  SET__FACT_TEST_RESIZE(32);
  SET__FACT_TEST_RESIZE(32);
  SET__FACT_TEST_RESIZE(64);
  SET__FACT_TEST_RESIZE(64);
  SET__FACT_TEST_RESIZE(128);
  SET__FACT_TEST_RESIZE(128);
  SET__FACT_TEST_RESIZE(256);
  SET__FACT_TEST_RESIZE(256);
  SET__FACT_TEST_RESIZE(512);
  SET__FACT_TEST_RESIZE(512);
  SET__FACT_TEST_RESIZE(1024);
  SET__FACT_TEST_RESIZE(1024);
  SET__FACT_TEST_RESIZE(2048);
  SET__FACT_TEST_RESIZE(2048);
  SET__FACT_TEST_RESIZE(4096);
  SET__FACT_TEST_RESIZE(4096);
  SET__FACT_TEST_RESIZE(8192);
  SET__FACT_TEST_RESIZE(8192);
  SET__FACT_TEST_RESIZE(16384);
  SET__FACT_TEST_RESIZE(16384);
  SET__FACT_TEST_RESIZE(32768);
  SET__FACT_TEST_RESIZE(32768);
  SET__FACT_TEST_RESIZE(65536);
  SET__FACT_TEST_RESIZE(65536);
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  set_clean__fact(&set);
}
TEST_CASE_END(set__fact_resize)
