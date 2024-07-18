/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "../libc3/buf.h"
#include "../libc3/facts.h"
#include "../libc3/facts_cursor.h"
#include "fact_test.h"
#include "test.h"

void facts_cursor_test (void);
TEST_CASE_PROTOTYPE(facts_cursor_init);
TEST_CASE_PROTOTYPE(facts_cursor_next);

void facts_cursor_test (void)
{
  TEST_CASE_RUN(facts_cursor_init);
  TEST_CASE_RUN(facts_cursor_next);
}

TEST_CASE(facts_cursor_init)
{
  s_facts_cursor cursor;
  uw i = 0;
  char *p[24] = {
    "0",
    "1",
    "10",
    "0x100",
    "0x10000",
    "0x100000000",
    "0x10000000000000000",
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
    "-1",
    "-10",
    "-0x100",
    "-0x10000",
    "-0x100000000",
    "-0x10000000000000000",
    NULL
  };
  s_fact fact[24];
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_spo, NULL, NULL),
          &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_pos, NULL, NULL),
          &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_osp, NULL, NULL),
          &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_spo, fact, fact),
          &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_pos, fact, fact),
          &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_osp, fact, fact),
          &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_spo, fact + 1,
                            fact + 1), &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_pos, fact + 1,
                            fact + 1), &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_osp, fact + 1,
                            fact + 1), &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_spo, fact,
                            fact + 6), &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_pos, fact,
                            fact + 6), &cursor);
  facts_cursor_clean(&cursor);
  TEST_EQ(facts_cursor_init(&facts, &cursor, facts.index_osp, fact,
                            fact + 6), &cursor);
  facts_cursor_clean(&cursor);
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_cursor_init)

TEST_CASE(facts_cursor_next)
{
  s_facts_cursor cursor;
  uw i = 0;
  char *p[24] = {
    "-0x10000000000000000",
    "-0x100000000",
    "-0x10000",
    "-0x100",
    "-10",
    "-1",
    "0",
    "1",
    "10",
    "0x100",
    "0x10000",
    "0x100000000",
    "0x10000000000000000",
    "[]",
    "[[], []]",
    "\"a\"",
    "A",
    ":a",
    "{:a, :b}",
    "{{:a, :b}, {:c, :d}}",
    "{{a, b}, {c, d}}",
    "{a, b}",
    "a",
    NULL
  };
  s_fact fact[24];
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  facts_cursor_init(&facts, &cursor, facts.index_spo, NULL, NULL);
  i = 0;
  while (p[i]) {
    FACT_TEST_EQ(facts_cursor_next(&cursor), fact + i);
    i++;
  }
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_pos, NULL, NULL);
  i = 0;
  while (p[i]) {
    FACT_TEST_EQ(facts_cursor_next(&cursor), fact + i);
    i++;
  }
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_osp, NULL, NULL);
  i = 0;
  while (p[i]) {
    FACT_TEST_EQ(facts_cursor_next(&cursor), fact + i);
    i++;
  }
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_spo, fact, fact);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_pos, fact, fact);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_osp, fact, fact);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_spo, fact + 1, fact + 1);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 1);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_pos, fact + 1, fact + 1);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 1);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_osp, fact + 1, fact + 1);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 1);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_spo, fact, fact + 6);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 1);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 2);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 3);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 4);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 5);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 6);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_pos, fact, fact + 6);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 1);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 2);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 3);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 4);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 5);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 6);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  facts_cursor_init(&facts, &cursor, facts.index_osp, fact, fact + 6);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 1);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 2);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 3);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 4);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 5);
  FACT_TEST_EQ(facts_cursor_next(&cursor), fact + 6);
  TEST_EQ(facts_cursor_next(&cursor), NULL);
  facts_cursor_clean(&cursor);
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_cursor_next)
