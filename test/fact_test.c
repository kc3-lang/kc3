/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <string.h>
#include "../libc3/fact.h"
#include "fact_test.h"
#include "test.h"

#define FACT_TEST_INSPECT(test, expected)                              \
  do {                                                                 \
    s_str str;                                                         \
    TEST_EQ(fact_inspect(test, &str), &str);                           \
    TEST_EQ(str.size, strlen(expected));                               \
    TEST_STRNCMP(str.ptr.p, (expected), str.size);                     \
  } while (0)

void fact_test ();
void fact_test_init ();
void fact_test_inspect ();

void fact_test ()
{
  fact_test_init();
  fact_test_inspect();
}

s_fact * fact_test_clean (s_fact *fact)
{
  assert(fact);
  tag_delete((s_tag *) fact->subject);
  tag_delete((s_tag *) fact->predicate);
  tag_delete((s_tag *) fact->object);
  return fact;
}

void fact_test_init ()
{
  s_tag sym[3];
  s_fact fact;
  tag_init_sym_1(sym, "A");
  tag_init_sym_1(sym + 1, "B");
  tag_init_sym_1(sym + 2, "C");
  TEST_EQ(fact_init(&fact, sym, sym + 1, sym + 2), &fact);
  TEST_EQ(fact.subject, sym);
  TEST_EQ(fact.predicate, sym + 1);
  TEST_EQ(fact.object, sym + 2);
}

s_fact * fact_test_init_1 (s_fact *fact, const s8 *tag)
{
  return fact_test_init_3(fact, tag, tag, tag);
}

s_fact * fact_test_init_3 (s_fact *fact, const s8 *subject,
                           const s8 *predicate, const s8 *object)
{
  assert(fact);
  fact->subject   = tag_new_1(subject);
  fact->predicate = tag_new_1(predicate);
  fact->object    = tag_new_1(object);
  return fact;
}

void fact_test_inspect ()
{
  s_tag tag[3];
  s_fact fact;
  tag_init_1(tag, "A");
  tag_init_1(tag + 1, "B");
  tag_init_1(tag + 2, "C");
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_INSPECT(&fact, "{A, B, C}");
}
