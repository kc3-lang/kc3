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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/tag.h"
#include "../libkc3/set_cursor__tag.h"
#include "../libkc3/set__tag.h"
#include "test.h"

#define SET__TAG_TEST_ADD(test, expected_count)                        \
  do {                                                                 \
    test_context("set_add__tag(" # test ") -> " # expected_count);     \
    TEST_ASSERT(set_add__tag(&set, (test)));                           \
    TEST_EQ(set.count, (expected_count));                              \
    test_context(NULL);                                                \
  } while (0)

#define SET__TAG_TEST_CURSOR(count)                                    \
  do {                                                                 \
    sw j = 0;                                                          \
    sw k = (count);                                                    \
    s_set_cursor__tag cursor;                                          \
    test_context("set_cursor");                                        \
    set_cursor_init__tag(&set, &cursor);                               \
    while (j < k) {                                                    \
      TEST_ASSERT(set_cursor_next__tag(&cursor));                      \
      j++;                                                             \
    }                                                                  \
    TEST_ASSERT(! set_cursor_next__tag(&cursor));                      \
    test_context(NULL);                                                \
  } while (0)

#define SET__TAG_TEST_INIT_CLEAN(max_)                                 \
  do {                                                                 \
    s_set__tag set;                                                    \
    test_context("set_init__tag(" # max_ ")");                         \
    TEST_EQ(set_init__tag(&set, max_), &set);                          \
    TEST_EQ(set.collisions, 0);                                        \
    TEST_EQ(set.count, 0);                                             \
    TEST_EQ(set.max, max_);                                            \
    set_clean__tag(&set);					       \
    test_context(NULL);                                                \
  } while (0)

#define SET__TAG_TEST_REMOVE(test, expected_count)                     \
  do {                                                                 \
    test_context("set_remove__tag(" # test ") -> " # expected_count);  \
    set_remove__tag(&set, (test));                                     \
    TEST_EQ(set.count, (expected_count));                              \
    test_context(NULL);                                                \
  } while (0)

#define SET__TAG_TEST_RESIZE(max_)                                     \
  do {                                                                 \
    test_context("set_resize__tag(" # max_ ")");                       \
    set_resize__tag(&set, (max_));                                     \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "\"a\"")));             \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, ":a")));                \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "A")));                 \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "a")));                 \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "[]")));                \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "[[], []]")));          \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "{:a, :b}")));          \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag,                         \
                                         "{{:a, :b}, {:c, :d}}")));    \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "{a, b}")));            \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "{{a, b}, {c, d}}")));  \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "0")));                 \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "1")));                 \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "10")));                \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "0x100")));             \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "0x10000")));           \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "0x100000000")));       \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag,                         \
                                         "0x10000000000000000")));     \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "-1")));                \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "-10")));               \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "-0x100")));            \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "-0x10000")));          \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag, "-0x100000000")));      \
    TEST_ASSERT(set_get__tag(&set, tag_1(&tag,                         \
                                         "-0x10000000000000000")));    \
    test_context(NULL);                                                \
  } while (0)

void set__tag_test (void);
TEST_CASE_PROTOTYPE(set__tag_add);
TEST_CASE_PROTOTYPE(set__tag_cursor);
TEST_CASE_PROTOTYPE(set__tag_init_clean);
TEST_CASE_PROTOTYPE(set__tag_remove);
TEST_CASE_PROTOTYPE(set__tag_resize);

void set__tag_test (void)
{
  TEST_CASE_RUN(set__tag_init_clean);
  TEST_CASE_RUN(set__tag_add);
  TEST_CASE_RUN(set__tag_remove);
  TEST_CASE_RUN(set__tag_resize);
  TEST_CASE_RUN(set__tag_cursor);
}

TEST_CASE(set__tag_add)
{
  uw i = 0;
  const char *p[] = {
    "false",
    "true",
    "'a'",
    "'\\n'",
    "\"a\"",
    ":a",
    "A",
    "a",
    "A.a(b, c)",
    "B.a(b, c)",
    "A.b(b, c)",
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
  s_tag tag;
  s_set__tag set;
  set_init__tag(&set, 64);
  tag_init(&tag);
  while (p[i]) {
    SET__TAG_TEST_ADD(tag_1(&tag, p[i]), i + 1);
    SET__TAG_TEST_ADD(&tag, i + 1);
    i++;
  }
  tag_clean(&tag);
  set_clean__tag(&set);
}
TEST_CASE_END(set__tag_add)

TEST_CASE(set__tag_cursor)
{
  sw i = 0;
  s_tag tag;
  s_set__tag set;
  set_init__tag(&set, 16);
  SET__TAG_TEST_CURSOR(i);
  set_add__tag(&set, tag_init_1(&tag, "\"a\""));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, ":a"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "A"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "a"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "[]"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "[[], []]"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "{:a, :b}"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "{{:a, :b}, {:c, :d}}"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "{a, b}"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "{{a, b}, {c, d}}"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "0"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "1"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "10"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "0x100"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "0x10000"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "0x100000000"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "0x10000000000000000"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "-1"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "-10"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "-0x100"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "-0x10000"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "-0x100000000"));
  SET__TAG_TEST_CURSOR(++i);
  set_add__tag(&set, tag_1(&tag, "-0x10000000000000000"));
  SET__TAG_TEST_CURSOR(++i);
  i = set.count;
  set_remove__tag(&set, tag_1(&tag, "\"a\""));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, ":a"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "A"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "a"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "[]"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "[[], []]"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "{:a, :b}"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "{{:a, :b}, {:c, :d}}"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "{a, b}"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "{{a, b}, {c, d}}"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "0"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "1"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "10"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "0x100"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "0x10000"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "0x100000000"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "0x10000000000000000"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "-1"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "-10"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "-0x100"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "-0x10000"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "-0x100000000"));
  SET__TAG_TEST_CURSOR(--i);
  set_remove__tag(&set, tag_1(&tag, "-0x10000000000000000"));
  SET__TAG_TEST_CURSOR(--i);
  TEST_EQ(i, 0);
  tag_clean(&tag);
  set_clean__tag(&set);
}
TEST_CASE_END(set__tag_cursor)

TEST_CASE(set__tag_init_clean)
{
  SET__TAG_TEST_INIT_CLEAN(1);
  SET__TAG_TEST_INIT_CLEAN(8);
  SET__TAG_TEST_INIT_CLEAN(10);
  SET__TAG_TEST_INIT_CLEAN(16);
  SET__TAG_TEST_INIT_CLEAN(20);
  SET__TAG_TEST_INIT_CLEAN(32);
  SET__TAG_TEST_INIT_CLEAN(40);
  SET__TAG_TEST_INIT_CLEAN(50);
  SET__TAG_TEST_INIT_CLEAN(64);
  SET__TAG_TEST_INIT_CLEAN(80);
  SET__TAG_TEST_INIT_CLEAN(90);
  SET__TAG_TEST_INIT_CLEAN(100);
  SET__TAG_TEST_INIT_CLEAN(128);
  SET__TAG_TEST_INIT_CLEAN(200);
  SET__TAG_TEST_INIT_CLEAN(256);
  SET__TAG_TEST_INIT_CLEAN(300);
  SET__TAG_TEST_INIT_CLEAN(500);
  SET__TAG_TEST_INIT_CLEAN(512);
  SET__TAG_TEST_INIT_CLEAN(600);
  SET__TAG_TEST_INIT_CLEAN(700);
  SET__TAG_TEST_INIT_CLEAN(800);
  SET__TAG_TEST_INIT_CLEAN(900);
  SET__TAG_TEST_INIT_CLEAN(1000);
  SET__TAG_TEST_INIT_CLEAN(1024);
  SET__TAG_TEST_INIT_CLEAN(10000);
  SET__TAG_TEST_INIT_CLEAN(16 * 1024);
  SET__TAG_TEST_INIT_CLEAN(100000);
  SET__TAG_TEST_INIT_CLEAN(128 * 1024);
  SET__TAG_TEST_INIT_CLEAN(1000000);
  SET__TAG_TEST_INIT_CLEAN(1024 * 1024);
}
TEST_CASE_END(set__tag_init_clean)

TEST_CASE(set__tag_remove)
{
  sw i;
  s_tag tag;
  s_set__tag set;
  set_init__tag(&set, 16);
  set_add__tag(&set, tag_init_1(&tag, "\"a\""));
  set_add__tag(&set, tag_1(&tag, ":a"));
  set_add__tag(&set, tag_1(&tag, "A"));
  set_add__tag(&set, tag_1(&tag, "a"));
  set_add__tag(&set, tag_1(&tag, "[]"));
  set_add__tag(&set, tag_1(&tag, "[[], []]"));
  set_add__tag(&set, tag_1(&tag, "{:a, :b}"));
  set_add__tag(&set, tag_1(&tag, "{{:a, :b}, {:c, :d}}"));
  set_add__tag(&set, tag_1(&tag, "{a, b}"));
  set_add__tag(&set, tag_1(&tag, "{{a, b}, {c, d}}"));
  set_add__tag(&set, tag_1(&tag, "0"));
  set_add__tag(&set, tag_1(&tag, "1"));
  set_add__tag(&set, tag_1(&tag, "10"));
  set_add__tag(&set, tag_1(&tag, "0x100"));
  set_add__tag(&set, tag_1(&tag, "0x10000"));
  set_add__tag(&set, tag_1(&tag, "0x100000000"));
  set_add__tag(&set, tag_1(&tag, "0x10000000000000000"));
  set_add__tag(&set, tag_1(&tag, "-1"));
  set_add__tag(&set, tag_1(&tag, "-10"));
  set_add__tag(&set, tag_1(&tag, "-0x100"));
  set_add__tag(&set, tag_1(&tag, "-0x10000"));
  set_add__tag(&set, tag_1(&tag, "-0x100000000"));
  set_add__tag(&set, tag_1(&tag, "-0x10000000000000000"));
  i = set.count;
  SET__TAG_TEST_REMOVE(tag_1(&tag, "\"a\""), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "\"a\""), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, ":a"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, ":a"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "A"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "A"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "a"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "a"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "[]"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "[]"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "[[], []]"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "[[], []]"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{:a, :b}"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{:a, :b}"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{{:a, :b}, {:c, :d}}"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{{:a, :b}, {:c, :d}}"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{a, b}"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{a, b}"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{{a, b}, {c, d}}"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "{{a, b}, {c, d}}"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "1"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "1"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "10"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "10"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x100"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x100"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x10000"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x10000"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x100000000"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x100000000"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x10000000000000000"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "0x10000000000000000"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-1"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-1"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-10"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-10"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x100"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x100"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x10000"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x10000"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x100000000"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x100000000"), i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x10000000000000000"), --i);
  SET__TAG_TEST_REMOVE(tag_1(&tag, "-0x10000000000000000"), i);
  tag_clean(&tag);
  set_clean__tag(&set);
}
TEST_CASE_END(set__tag_remove)

TEST_CASE(set__tag_resize)
{
  s_tag tag;
  s_set__tag set;
  set_init__tag(&set, 64);
  set_add__tag(&set, tag_init_1(&tag, "\"a\""));
  set_add__tag(&set, tag_1(&tag, "\"a\""));
  set_add__tag(&set, tag_1(&tag, ":a"));
  set_add__tag(&set, tag_1(&tag, ":a"));
  set_add__tag(&set, tag_1(&tag, "A"));
  set_add__tag(&set, tag_1(&tag, "A"));
  set_add__tag(&set, tag_1(&tag, "a"));
  set_add__tag(&set, tag_1(&tag, "a"));
  set_add__tag(&set, tag_1(&tag, "[]"));
  set_add__tag(&set, tag_1(&tag, "[]"));
  set_add__tag(&set, tag_1(&tag, "[[], []]"));
  set_add__tag(&set, tag_1(&tag, "[[], []]"));
  set_add__tag(&set, tag_1(&tag, "{:a, :b}"));
  set_add__tag(&set, tag_1(&tag, "{:a, :b}"));
  set_add__tag(&set, tag_1(&tag, "{{:a, :b}, {:c, :d}}"));
  set_add__tag(&set, tag_1(&tag, "{{:a, :b}, {:c, :d}}"));
  set_add__tag(&set, tag_1(&tag, "{a, b}"));
  set_add__tag(&set, tag_1(&tag, "{a, b}"));
  set_add__tag(&set, tag_1(&tag, "{{a, b}, {c, d}}"));
  set_add__tag(&set, tag_1(&tag, "{{a, b}, {c, d}}"));
  set_add__tag(&set, tag_1(&tag, "0"));
  set_add__tag(&set, tag_1(&tag, "1"));
  set_add__tag(&set, tag_1(&tag, "10"));
  set_add__tag(&set, tag_1(&tag, "0x100"));
  set_add__tag(&set, tag_1(&tag, "0x10000"));
  set_add__tag(&set, tag_1(&tag, "0x100000000"));
  set_add__tag(&set, tag_1(&tag, "0x10000000000000000"));
  set_add__tag(&set, tag_1(&tag, "-1"));
  set_add__tag(&set, tag_1(&tag, "-10"));
  set_add__tag(&set, tag_1(&tag, "-0x100"));
  set_add__tag(&set, tag_1(&tag, "-0x10000"));
  set_add__tag(&set, tag_1(&tag, "-0x100000000"));
  set_add__tag(&set, tag_1(&tag, "-0x10000000000000000"));
  SET__TAG_TEST_RESIZE(1);
  SET__TAG_TEST_RESIZE(1);
  SET__TAG_TEST_RESIZE(2);
  SET__TAG_TEST_RESIZE(2);
  SET__TAG_TEST_RESIZE(4);
  SET__TAG_TEST_RESIZE(4);
  SET__TAG_TEST_RESIZE(8);
  SET__TAG_TEST_RESIZE(8);
  SET__TAG_TEST_RESIZE(16);
  SET__TAG_TEST_RESIZE(16);
  SET__TAG_TEST_RESIZE(32);
  SET__TAG_TEST_RESIZE(32);
  SET__TAG_TEST_RESIZE(64);
  SET__TAG_TEST_RESIZE(64);
  SET__TAG_TEST_RESIZE(128);
  SET__TAG_TEST_RESIZE(128);
  SET__TAG_TEST_RESIZE(256);
  SET__TAG_TEST_RESIZE(256);
  SET__TAG_TEST_RESIZE(512);
  SET__TAG_TEST_RESIZE(512);
  SET__TAG_TEST_RESIZE(1024);
  SET__TAG_TEST_RESIZE(1024);
  SET__TAG_TEST_RESIZE(2048);
  SET__TAG_TEST_RESIZE(2048);
  SET__TAG_TEST_RESIZE(4096);
  SET__TAG_TEST_RESIZE(4096);
  SET__TAG_TEST_RESIZE(8192);
  SET__TAG_TEST_RESIZE(8192);
  SET__TAG_TEST_RESIZE(16384);
  SET__TAG_TEST_RESIZE(16384);
  SET__TAG_TEST_RESIZE(32768);
  SET__TAG_TEST_RESIZE(32768);
  SET__TAG_TEST_RESIZE(65536);
  SET__TAG_TEST_RESIZE(65536);
  tag_clean(&tag);
  set_clean__tag(&set);
}
TEST_CASE_END(set__tag_resize)
