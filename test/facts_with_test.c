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
#include "../libc3/facts.h"
#include "../libc3/facts_cursor.h"
#include "../libc3/facts_with.h"
#include "../libc3/facts_with_cursor.h"
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
             { tag_init_var(&subject),
               tag_init_var(&predicate),
               tag_init_var(&object),
               NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 1, tag + 3);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 4, tag + 3);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag + 6, tag + 7, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag,
                                            tag_init_var(&predicate),
                                            tag_init_var(&object),
                                            NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 1, tag + 3);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 4, tag + 3);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag,
                                            tag + 1,
                                            tag_init_var(&object),
                                            NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 1, tag + 3);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag, tag + 1, tag + 2,
                                            NULL, NULL });
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag + 5, tag + 1, tag + 2,
                                            NULL, NULL });
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag_init_var(&subject),
                                            tag_init_var(&predicate),
                                            tag_init_var(&object),
                                            NULL,
                                            &subject,
                                            tag + 1,
                                            &object,
                                            NULL, NULL });
  TEST_ASSERT(facts_with_cursor_next(&cursor));
  TAG_TEST_EQ(&subject, tag);
  TAG_TEST_EQ(&predicate, tag + 1);
  TAG_TEST_EQ(&object, tag + 2);
  TEST_ASSERT(facts_with_cursor_next(&cursor));
  TAG_TEST_EQ(&subject, tag);
  TAG_TEST_EQ(&predicate, tag + 1);
  TAG_TEST_EQ(&object, tag + 3);
  TEST_ASSERT(facts_with_cursor_next(&cursor));
  TAG_TEST_EQ(&subject, tag);
  TAG_TEST_EQ(&predicate, tag + 4);
  TAG_TEST_EQ(&object, tag + 3);
  TEST_ASSERT(facts_with_cursor_next(&cursor));
  TAG_TEST_EQ(&subject, tag + 5);
  TAG_TEST_EQ(&predicate, tag + 1);
  TAG_TEST_EQ(&object, tag + 2);
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag, tag + 1, tag + 2,
                                                 tag + 1, tag + 3,
                                                 NULL,
                                            tag + 5, tag + 1, tag + 2,
                                            NULL, NULL });
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag + 3, tag, tag + 1,
                                                     tag, tag + 2,
                                                     NULL,
                                            tag + 4, tag + 1, tag + 2,
                                            NULL, NULL });
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag_init_var(&subject),
                                            tag_init_var(&predicate),
                                            tag_init_var(&object),
                                            NULL,
                                            tag + 3, tag, tag + 1,
                                                     tag, tag + 2,
                                                     NULL,
                                            tag + 4, tag + 1, tag + 2,
                                            NULL, NULL });
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_with(&facts, &cursor, (s_tag *[]) { tag_init_var(&subject),
                                            tag_init_var(&predicate),
                                            tag_init_var(&object),
                                            NULL,
                                            tag, tag + 1, tag + 2,
                                                 tag, tag + 2,
                                                 NULL,
                                            tag + 4, tag + 1, tag + 2,
                                            NULL, NULL });
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  TEST_ASSERT(! facts_with_cursor_next(&cursor));
  facts_with_cursor_clean(&cursor);
  facts_clean(&facts);
}
TEST_CASE_END(facts_with_)

TEST_CASE(facts_with_tags)
{
  s_facts_cursor cursor;
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
                  tag_init_var(&subject),
                  tag_init_var(&predicate),
                  tag_init_var(&object));
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 1, tag + 3);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 4, tag + 3);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag + 5, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag + 6, tag + 7, tag + 2);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  FACT_TEST_EQ(NULL, facts_cursor_next(&cursor));
  facts_with_tags(&facts, &cursor,
                  tag,
                  tag_init_var(&predicate),
                  tag_init_var(&object));
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 1, tag + 3);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 4, tag + 3);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  FACT_TEST_EQ(NULL, facts_cursor_next(&cursor));
  facts_with_tags(&facts, &cursor,
                  tag,
                  tag + 1,
                  tag_init_var(&object));
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  fact_init(&fact, tag, tag + 1, tag + 3);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  FACT_TEST_EQ(NULL, facts_cursor_next(&cursor));
  facts_with_tags(&facts, &cursor, tag, tag + 1, tag + 2);
  fact_init(&fact, tag, tag + 1, tag + 2);
  FACT_TEST_EQ(&fact, facts_cursor_next(&cursor));
  FACT_TEST_EQ(NULL, facts_cursor_next(&cursor));
  facts_clean(&facts);
}
TEST_CASE_END(facts_with_tags)
