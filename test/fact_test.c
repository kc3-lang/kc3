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
#include <string.h>
#include "../libkc3/fact.h"
#include "../libkc3/str.h"
#include "fact_test.h"
#include "test.h"

void fact_test (void);
TEST_CASE_PROTOTYPE(fact_init);

void fact_test (void)
{
  TEST_CASE_RUN(fact_init);
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
  fact->id = 0;
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
  tag_init_psym(sym, sym_1("A"));
  tag_init_psym(sym + 1, sym_1("B"));
  tag_init_psym(sym + 2, sym_1("C"));
  TEST_EQ(fact_init(&fact, sym, sym + 1, sym + 2), &fact);
  TEST_EQ(fact.subject, sym);
  TEST_EQ(fact.predicate, sym + 1);
  TEST_EQ(fact.object, sym + 2);
}
TEST_CASE_END(fact_init)
