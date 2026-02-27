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
#include "../libkc3/facts.h"
#include "../libkc3/facts_cursor.h"
#include "../libkc3/facts_with.h"
#include "../libkc3/facts_with_cursor.h"
#include "fact_test.h"
#include "tag_test.h"
#include "test.h"

void facts_with_test (void);
TEST_CASE_PROTOTYPE(facts_with_);
TEST_CASE_PROTOTYPE(facts_with_tags);

void facts_with_test (void)
{
  TEST_CASE_RUN(facts_with_tags);
  TEST_CASE_RUN(facts_with_);
}

TEST_CASE(facts_with_)
{
  s_facts_with_cursor cursor;
  s_fact *f;
  s_fact fact;
  s_facts facts;
  sw i = 0;
  s_tag object;
  const char *p[9] = { "A", "B", "C", "D", "E", "F", "G", "H", NULL };
  s_tag predicate;
  s_tag subject;
  s_tag tag[8];
  while (p[i]) {
    tag_init_1(tag + i, p[i]);
    i++;
  }
  facts_init(&facts);
  facts_add_tags(&facts, tag, tag + 1, tag + 2);
  facts_add_tags(&facts, tag, tag + 1, tag + 3);
  facts_add_tags(&facts, tag, tag + 4, tag + 3);
  facts_add_tags(&facts, tag + 5, tag + 1, tag + 2);
  facts_add_tags(&facts, tag + 6, tag + 7, tag + 2);
  i = 0;
  facts_with(&facts, &cursor, (s_tag *[])
             { tag_init_pvar(&subject, &g_sym_Tag),
               tag_init_pvar(&predicate, &g_sym_Tag),
               tag_init_pvar(&object, &g_sym_Tag),
               NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 1, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 4, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag + 6, tag + 7, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_with(&facts, &cursor, (s_tag *[]) { tag,
                                            tag_init_pvar(&predicate,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&object,
                                                          &g_sym_Tag),
                                            NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 1, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 4, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_with(&facts, &cursor, (s_tag *[]) { tag,
                                            tag + 1,
                                            tag_init_pvar(&object,
                                                          &g_sym_Tag),
                                            NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 1, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  tag_clean(&object);
  facts_with(&facts, &cursor, (s_tag *[]) { tag, tag + 1, tag + 2,
                                            NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag + 5, tag + 1, tag + 2,
                                            NULL, NULL });
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag_init_pvar(&subject,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&predicate,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&object,
                                                          &g_sym_Tag),
                                            NULL,
                                            &subject,
                                            tag + 1,
                                            &object,
                                            NULL, NULL });
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(f);
  TAG_TEST_EQ(&subject.data.td_pvar->tag, tag);
  TAG_TEST_EQ(&predicate.data.td_pvar->tag, tag + 1);
  TAG_TEST_EQ(&object.data.td_pvar->tag, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(f);
  TAG_TEST_EQ(&subject.data.td_pvar->tag, tag);
  TAG_TEST_EQ(&predicate.data.td_pvar->tag, tag + 1);
  TAG_TEST_EQ(&object.data.td_pvar->tag, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(f);
  TAG_TEST_EQ(&subject.data.td_pvar->tag, tag);
  TAG_TEST_EQ(&predicate.data.td_pvar->tag, tag + 4);
  TAG_TEST_EQ(&object.data.td_pvar->tag, tag + 3);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(f);
  TAG_TEST_EQ(&subject.data.td_pvar->tag, tag + 5);
  TAG_TEST_EQ(&predicate.data.td_pvar->tag, tag + 1);
  TAG_TEST_EQ(&object.data.td_pvar->tag, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_with(&facts, &cursor, (s_tag *[]) { tag, tag + 1, tag + 2,
                                                 tag + 1, tag + 3,
                                                 NULL,
                                            tag + 5, tag + 1, tag + 2,
                                            NULL, NULL });
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag + 3, tag, tag + 1,
                                                     tag, tag + 2,
                                                     NULL,
                                            tag + 4, tag + 1, tag + 2,
                                            NULL, NULL });
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag_init_pvar(&subject,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&predicate,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&object,
                                                          &g_sym_Tag),
                                            NULL,
                                            tag + 3, tag, tag + 1,
                                                     tag, tag + 2,
                                                     NULL,
                                            tag + 4, tag + 1, tag + 2,
                                            NULL, NULL });
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_with(&facts, &cursor, (s_tag *[]) { tag_init_pvar(&subject,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&predicate,
                                                          &g_sym_Tag),
                                            tag_init_pvar(&object,
                                                          &g_sym_Tag),
                                            NULL,
                                            tag, tag + 1, tag + 2,
                                                 tag, tag + 2,
                                                 NULL,
                                            tag + 4, tag + 1, tag + 2,
                                            NULL, NULL });
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_with_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_with_cursor_clean(&cursor);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_clean(&facts);
}
TEST_CASE_END(facts_with_)

TEST_CASE(facts_with_tags)
{
  s_facts_cursor cursor;
  s_fact *f;
  s_fact fact;
  s_facts facts;
  sw i = 0;
  s_tag object;
  const char *p[9] = { "A", "B", "C", "D", "E", "F", "G", "H", NULL };
  s_tag predicate;
  s_tag subject;
  s_tag tag[8];
  while (p[i]) {
    tag_init_1(tag + i, p[i]);
    i++;
  }
  facts_init(&facts);
  facts_add_tags(&facts, tag, tag + 1, tag + 2);
  facts_add_tags(&facts, tag, tag + 1, tag + 3);
  facts_add_tags(&facts, tag, tag + 4, tag + 3);
  facts_add_tags(&facts, tag + 5, tag + 1, tag + 2);
  facts_add_tags(&facts, tag + 6, tag + 7, tag + 2);
  i = 0;
  facts_with_tags(&facts, &cursor,
                  tag_init_pvar(&subject, &g_sym_Tag),
                  tag_init_pvar(&predicate, &g_sym_Tag),
                  tag_init_pvar(&object, &g_sym_Tag));
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 1, tag + 3);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 4, tag + 3);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag + 6, tag + 7, tag + 2);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_with_tags(&facts, &cursor,
                  tag,
                  tag_init_pvar(&predicate, &g_sym_Tag),
                  tag_init_pvar(&object, &g_sym_Tag));
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 1, tag + 3);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 4, tag + 3);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_with_tags(&facts, &cursor,
                  tag,
                  tag + 1,
                  tag_init_pvar(&object, &g_sym_Tag));
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  fact_init(&fact, tag, tag + 1, tag + 3);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  tag_clean(&object);
  facts_with_tags(&facts, &cursor, tag, tag + 1, tag + 2);
  fact_init(&fact, tag, tag + 1, tag + 2);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  FACT_TEST_EQ(&fact, f);
  TEST_EQ(facts_cursor_next(&cursor, &f), &f);
  TEST_ASSERT(! f);
  facts_clean(&facts);
}
TEST_CASE_END(facts_with_tags)
