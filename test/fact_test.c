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
#include <string.h>
#include "../libc3/fact.h"
#include "../libc3/str.h"
#include "fact_test.h"
#include "test.h"

#define FACT_TEST_INSPECT(test, expected)                              \
  do {                                                                 \
    s_str str;                                                         \
    TEST_EQ(fact_inspect(test, &str), &str);                           \
    TEST_EQ(str.size, strlen(expected));                               \
    if (g_test_last_ok)						       \
      TEST_STRNCMP(str.ptr.p, (expected), str.size);		       \
    str_clean(&str);						       \
  } while (0)

void fact_test (void);
TEST_CASE_PROTOTYPE(fact_init);
TEST_CASE_PROTOTYPE(fact_inspect);

void fact_test (void)
{
  TEST_CASE_RUN(fact_init);
  TEST_CASE_RUN(fact_inspect);
}

void fact_test_clean_1 (s_fact *fact)
{
  assert(fact);
  tag_delete((s_tag *) fact->subject);
}

void fact_test_clean_3 (s_fact *fact)
{
  assert(fact);
  tag_delete((s_tag *) fact->subject);
  tag_delete((s_tag *) fact->predicate);
  tag_delete((s_tag *) fact->object);
}

s_fact * fact_test_init_1 (s_fact *fact, const char *tag)
{
  assert(fact);
  fact->subject = fact->predicate = fact->object = tag_new_1(tag);
  return fact;
}

s_fact * fact_test_init_3 (s_fact *fact, const char *subject,
                           const char *predicate, const char *object)
{
  assert(fact);
  fact->subject   = tag_new_1(subject);
  fact->predicate = tag_new_1(predicate);
  fact->object    = tag_new_1(object);
  return fact;
}

TEST_CASE(fact_init)
{
  s_tag sym[3];
  s_fact fact;
  tag_init_sym(sym, sym_1("A"));
  tag_init_sym(sym + 1, sym_1("B"));
  tag_init_sym(sym + 2, sym_1("C"));
  TEST_EQ(fact_init(&fact, sym, sym + 1, sym + 2), &fact);
  TEST_EQ(fact.subject, sym);
  TEST_EQ(fact.predicate, sym + 1);
  TEST_EQ(fact.object, sym + 2);
}
TEST_CASE_END(fact_init)

TEST_CASE(fact_inspect)
{
  s_tag tag[3];
  s_fact fact;
  tag_init_1(tag, "A");
  tag_init_1(tag + 1, "B");
  tag_init_1(tag + 2, "C");
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_INSPECT(&fact, "{A, B, C}");
}
TEST_CASE_END(fact_inspect)
