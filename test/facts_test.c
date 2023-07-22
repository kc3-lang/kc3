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
#include <stdio.h>
#include <unistd.h>
#include "../libc3/buf.h"
#include "../libc3/compare.h"
#include "../libc3/facts.h"
#include "../libc3/log.h"
#include "fact_test.h"
#include "test.h"

void facts_test ();
TEST_CASE_PROTOTYPE(facts_add);
TEST_CASE_PROTOTYPE(facts_dump_file);
TEST_CASE_PROTOTYPE(facts_find);
TEST_CASE_PROTOTYPE(facts_init_clean);
TEST_CASE_PROTOTYPE(facts_load);
TEST_CASE_PROTOTYPE(facts_log_add);
TEST_CASE_PROTOTYPE(facts_log_remove);
TEST_CASE_PROTOTYPE(facts_new_delete);
TEST_CASE_PROTOTYPE(facts_open_file);
TEST_CASE_PROTOTYPE(facts_remove);
TEST_CASE_PROTOTYPE(facts_save);

void facts_test ()
{
  TEST_CASE_RUN(facts_init_clean);
  TEST_CASE_RUN(facts_new_delete);
  TEST_CASE_RUN(facts_add);
  TEST_CASE_RUN(facts_remove);
  TEST_CASE_RUN(facts_find);
  TEST_CASE_RUN(facts_log_add);
  TEST_CASE_RUN(facts_log_remove);
  TEST_CASE_RUN(facts_dump_file);
  TEST_CASE_RUN(facts_load);
  TEST_CASE_RUN(facts_save);
  TEST_CASE_RUN(facts_open_file);
}

TEST_CASE(facts_add)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_fact *pf;
  s_fact fact[24];
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    TEST_ASSERT((pf = facts_add_fact(&facts, fact + i)));
    TEST_EQ(facts.tags.count, i + 1);
    TEST_EQ(facts.facts.count, i + 1);
    TEST_EQ(compare_tag(fact[i].subject, pf->subject), 0);
    TEST_EQ(compare_tag(fact[i].predicate, pf->predicate), 0);
    TEST_EQ(compare_tag(fact[i].object, pf->object), 0);
    TEST_ASSERT((pf = facts_add_fact(&facts, fact + i)));
    TEST_EQ(facts.tags.count, i + 1);
    TEST_EQ(facts.facts.count, i + 1);
    TEST_EQ(compare_tag(fact[i].subject, pf->subject), 0);
    TEST_EQ(compare_tag(fact[i].predicate, pf->predicate), 0);
    TEST_EQ(compare_tag(fact[i].object, pf->object), 0);
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_add)

TEST_CASE(facts_dump_file)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  facts_dump_file(&facts, "facts_test_dump_file.facts");
  test_file_compare("facts_test_dump_file.facts",
                    "facts_test_dump_file.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_dump_file.facts");
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_dump_file)

TEST_CASE(facts_find)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_facts facts;
  s_fact *pf;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    TEST_EQ(facts_find_fact(&facts, fact + i), NULL);
    TEST_EQ(facts_find_fact(&facts, fact + i), NULL);
    facts_add_fact(&facts, fact + i);
    TEST_ASSERT((pf = facts_find_fact(&facts, fact + i)));
    TEST_EQ(compare_tag(fact[i].subject, pf->subject), 0);
    TEST_EQ(compare_tag(fact[i].predicate, pf->predicate), 0);
    TEST_EQ(compare_tag(fact[i].object, pf->object), 0);
    i++;
  }
  while (i--) {
    facts_remove_fact(&facts, fact + i);
    TEST_EQ(facts_find_fact(&facts, fact + i), NULL);
    TEST_EQ(facts_find_fact(&facts, fact + i), NULL);
    fact_test_clean_1(fact + i);
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_find)

TEST_CASE(facts_init_clean)
{
  s_facts facts;
  TEST_EQ(facts_init(&facts), &facts);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(facts.facts.count, 0);
  TEST_EQ(facts.log, NULL);
  facts_clean(&facts);
  test_ok();
  TEST_EQ(facts_init(&facts), &facts);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(facts.facts.count, 0);
  TEST_EQ(facts.log, NULL);
  facts_clean(&facts);
  test_ok();
}
TEST_CASE_END(facts_init_clean)

TEST_CASE(facts_load)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_fact fact;
  s_facts facts;
  s_str path;
  facts_init(&facts);
  str_init_1(&path, NULL, "facts_test_load_file.facts");
  TEST_EQ(facts_load_file(&facts, &path), 681);
  TEST_EQ(facts_count(&facts), 23);
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_ASSERT(facts_find_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_load)

TEST_CASE(facts_log_add)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_facts facts;
  FILE *fp;
  fp = fopen("facts_test_log_add.facts", "w");
  facts_init(&facts);
  facts.log = log_new();
  log_open(facts.log, fp);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
  fclose(fp);
  test_file_compare("facts_test_log_add.facts",
                    "facts_test_log_add.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_log_add.facts");
}
TEST_CASE_END(facts_log_add)

TEST_CASE(facts_log_remove)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_facts facts;
  FILE *fp;
  fp = fopen("facts_test_log_remove.facts", "w");
  facts_init(&facts);
  facts.log = log_new();
  log_open(facts.log, fp);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  while (i--) {
    facts_remove_fact(&facts, fact + i);
  }
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
  fclose(fp);
  test_file_compare("facts_test_log_remove.facts",
                    "facts_test_log_remove.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_log_remove.facts");
}
TEST_CASE_END(facts_log_remove)

TEST_CASE(facts_new_delete)
{
  s_facts *facts;
  s_buf buf;
  TEST_ASSERT((facts = facts_new(NULL)));
  if (g_test_last_ok) {
    TEST_EQ(facts->tags.count, 0);
    TEST_EQ(facts->facts.count, 0);
    TEST_EQ(facts->log, NULL);
    facts_delete(facts);
    test_ok();
  }
  TEST_ASSERT((facts = facts_new(&buf)));
  if (g_test_last_ok) {
    TEST_EQ(facts->tags.count, 0);
    TEST_EQ(facts->facts.count, 0);
    TEST_EQ(facts->log, NULL);
    facts_delete(facts);
    test_ok();
  }
}
TEST_CASE_END(facts_new_delete)

TEST_CASE(facts_open_file)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s8 *q[24] = {
    "\"b\"",
    ":b",
    "B",
    "b",
    "(())",
    "((()), ())",
    "{:b, :b}",
    "{{:b, :b}, {:c, :d}}",
    "{b, b}",
    "{{b, b}, {c, d}}",
    "2",
    "3",
    "11",
    "0x101",
    "0x10001",
    "0x100000001",
    "0x10000000000000001",
    "-2",
    "-11",
    "-0x101",
    "-0x10001",
    "-0x100000001",
    "-0x10000000000000001",
    NULL
  };
  sw r;
  s_fact fact;
  s_facts facts;
  s_str path;
  if ((r = system("cp facts_test_open_file.1.in.facts facts_test_open_file.1.facts")) < 0)
    err(1, "%s:%i: cp", __FILE__, __LINE__);
  if (r > 0)
    errx(1, "%s:%i: cp", __FILE__, __LINE__);
  facts_init(&facts);
  str_init_1(&path, NULL, "facts_test_open_file.1.facts");
  TEST_EQ(facts_open_file(&facts, &path), 720);
  TEST_EQ(facts_count(&facts), 23);
  i = 0;
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_ASSERT(facts_find_fact(&facts, &fact));
    TEST_ASSERT(facts_remove_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  i = 0;
  while (q[i]) {
    fact_test_init_1(&fact, q[i]);
    TEST_ASSERT(! facts_find_fact(&facts, &fact));
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  facts_close(&facts);
  facts_clean(&facts);
  test_file_compare("facts_test_open_file.1.facts",
                    "facts_test_open_file.1.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_open_file.1.facts");
  facts_init(&facts);
  if ((r = system("cp facts_test_open_file.2.in.facts facts_test_open_file.2.facts")) < 0)
    err(1, "%s:%i: cp", __FILE__, __LINE__);
  if (r > 0)
    errx(1, "%s:%i: cp", __FILE__, __LINE__);
  str_init_1(&path, NULL, "facts_test_open_file.2.facts");
  TEST_EQ(facts_open_file(&facts, &path), 1453);
  TEST_EQ(facts_count(&facts), 46);
  i = 0;
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_ASSERT(facts_find_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  i = 0;
  while (q[i]) {
    fact_test_init_1(&fact, q[i]);
    TEST_ASSERT(facts_find_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  facts_close(&facts);
  facts_clean(&facts);
  test_file_compare("facts_test_open_file.2.facts",
                    "facts_test_open_file.2.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_open_file.2.facts");
  facts_init(&facts);
  if ((r = system("cp facts_test_open_file.3.in.facts facts_test_open_file.3.facts")) < 0)
    err(1, "%s:%i: cp", __FILE__, __LINE__);
  if (r > 0)
    errx(1, "%s:%i: cp", __FILE__, __LINE__);
  str_init_1(&path, NULL, "facts_test_open_file.3.facts");
  TEST_EQ(facts_open_file(&facts, &path), 1510);
  TEST_EQ(facts_count(&facts), 0);
  i = 0;
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_ASSERT(! facts_find_fact(&facts, &fact));
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  i = 0;
  while (q[i]) {
    fact_test_init_1(&fact, q[i]);
    TEST_ASSERT(! facts_find_fact(&facts, &fact));
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  facts_close(&facts);
  facts_clean(&facts);
  test_file_compare("facts_test_open_file.3.facts",
                    "facts_test_open_file.3.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_open_file.3.facts");
}
TEST_CASE_END(facts_open_file)

TEST_CASE(facts_remove)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  while (i--) {
    TEST_EQ(facts_remove_fact(&facts, fact + i), true);
    TEST_EQ(facts.tags.count, i);
    TEST_EQ(facts.facts.count, i);
    TEST_EQ(facts_remove_fact(&facts, fact + i), false);
    TEST_EQ(facts.tags.count, i);
    TEST_EQ(facts.facts.count, i);
    fact_test_clean_1(fact + i);
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_remove)

TEST_CASE(facts_save)
{
  uw i = 0;
  s8 *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "()",
    "((), ())",
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
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  facts_save_file(&facts, "facts_test_save.facts");
  test_file_compare("facts_test_save.facts",
                    "facts_test_save.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_save.facts");
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_save)
