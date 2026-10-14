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
#include <stdio.h>
#include <unistd.h>
#include "../libkc3/buf.h"
#include "../libkc3/compare.h"
#include "../libkc3/facts.h"
#include "../libkc3/facts_transaction.h"
#include "../libkc3/file.h"
#include "../libkc3/log.h"
#include "../libkc3/marshall.h"
#include "../libkc3/skiplist__fact.h"
#include "fact_test.h"
#include "test.h"

void facts_test (void);
TEST_CASE_PROTOTYPE(facts_add);
#ifdef NDEBUG
TEST_CASE_PROTOTYPE(facts_close_no_log);
#endif
TEST_CASE_PROTOTYPE(facts_dump_file);
TEST_CASE_PROTOTYPE(facts_find);
TEST_CASE_PROTOTYPE(facts_init_clean);
TEST_CASE_PROTOTYPE(facts_load);
TEST_CASE_PROTOTYPE(facts_log_add);
TEST_CASE_PROTOTYPE(facts_log_remove);
TEST_CASE_PROTOTYPE(facts_new_delete);
TEST_CASE_PROTOTYPE(facts_open_file);
TEST_CASE_PROTOTYPE(facts_open_file_binary_round_trip);
TEST_CASE_PROTOTYPE(facts_remove);
TEST_CASE_PROTOTYPE(facts_save);
TEST_CASE_PROTOTYPE(facts_save_binary);
TEST_CASE_PROTOTYPE(facts_transaction_add);
TEST_CASE_PROTOTYPE(facts_transaction_remove);
TEST_CASE_PROTOTYPE(facts_transaction_replace);
TEST_CASE_PROTOTYPE(facts_transaction_nested);
TEST_CASE_PROTOTYPE(facts_transaction_publish);
TEST_CASE_PROTOTYPE(facts_transaction_remote);
TEST_CASE_PROTOTYPE(facts_transaction_reopen);
TEST_CASE_PROTOTYPE(facts_transaction_remove_wildcard);

void facts_test (void)
{
  TEST_CASE_RUN(facts_init_clean);
#ifdef NDEBUG
  TEST_CASE_RUN(facts_close_no_log);
#endif
  TEST_CASE_RUN(facts_new_delete);
  TEST_CASE_RUN(facts_add);
  TEST_CASE_RUN(facts_remove);
  TEST_CASE_RUN(facts_find);
  TEST_CASE_RUN(facts_log_add);
  TEST_CASE_RUN(facts_log_remove);
  TEST_CASE_RUN(facts_dump_file);
  TEST_CASE_RUN(facts_load);
  TEST_CASE_RUN(facts_save);
  TEST_CASE_RUN(facts_save_binary);
  TEST_CASE_RUN(facts_open_file);
  TEST_CASE_RUN(facts_open_file_binary_round_trip);
  TEST_CASE_RUN(facts_transaction_add);
  TEST_CASE_RUN(facts_transaction_remove);
  TEST_CASE_RUN(facts_transaction_replace);
  TEST_CASE_RUN(facts_transaction_nested);
  TEST_CASE_RUN(facts_transaction_publish);
  TEST_CASE_RUN(facts_transaction_remote);
  TEST_CASE_RUN(facts_transaction_reopen);
  TEST_CASE_RUN(facts_transaction_remove_wildcard);
}

TEST_CASE(facts_transaction_add)
{
  s_fact fact = {0};
  s_fact *found;
  s_facts facts;
  s_facts_transaction transaction;
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_1(&fact, "\"rollback add\""));
  TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  TEST_EQ(facts_count(&facts), 1);
  TEST_EQ(facts_transaction_rollback(&facts, &transaction), &facts);
  TEST_EQ(facts_count(&facts), 0);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(facts.transaction, NULL);
  TEST_EQ(facts_find_fact(&facts, &fact, &found), &found);
  TEST_EQ(found, NULL);
  TEST_EQ(skiplist_find__fact(facts.index_spo, &fact), NULL);
  TEST_EQ(skiplist_find__fact(facts.index_pos, &fact), NULL);
  TEST_EQ(skiplist_find__fact(facts.index_osp, &fact), NULL);
  facts_transaction_clean(&transaction);
  TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
  fact.id = 123;
  TEST_ASSERT(facts_add_fact_id(&facts, &fact));
  TEST_EQ(facts_transaction_rollback(&facts, &transaction), &facts);
  TEST_EQ(facts_count(&facts), 0);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(skiplist_find__fact(facts.index, &fact), NULL);
  fact_test_clean_1(&fact);
  facts_clean(&facts);
}
TEST_CASE_END(facts_transaction_add)

TEST_CASE(facts_transaction_remove)
{
  bool removed;
  s_fact fact = {0};
  s_fact *found;
  s_facts facts;
  uw id;
  s_facts_transaction transaction;
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_1(&fact, "\"rollback remove\""));
  TEST_ASSERT((found = facts_add_fact(&facts, &fact)));
  id = found->id;
  TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
  TEST_EQ(facts_remove_fact(&facts, found, &removed), &removed);
  TEST_ASSERT(removed);
  TEST_EQ(facts_remove_fact(&facts, &fact, &removed), &removed);
  TEST_EQ(removed, false);
  fact_test_clean_1(&fact);
  TEST_EQ(facts_transaction_rollback(&facts, &transaction), &facts);
  TEST_EQ(facts_count(&facts), 1);
  TEST_EQ(facts.tags.count, 1);
  TEST_ASSERT(fact_test_init_1(&fact, "\"rollback remove\""));
  TEST_EQ(facts_find_fact(&facts, &fact, &found), &found);
  TEST_ASSERT(found);
  TEST_EQ(found->id, id);
  TEST_ASSERT(skiplist_find__fact(facts.index, found));
  TEST_ASSERT(skiplist_find__fact(facts.index_spo, found));
  TEST_ASSERT(skiplist_find__fact(facts.index_pos, found));
  TEST_ASSERT(skiplist_find__fact(facts.index_osp, found));
  facts_transaction_clean(&transaction);
  fact_test_clean_1(&fact);
  facts_clean(&facts);
}
TEST_CASE_END(facts_transaction_remove)

TEST_CASE(facts_transaction_replace)
{
  s_fact old[2] = {0};
  s_fact replacement = {0};
  s_fact *found;
  s_facts facts;
  uw id[2];
  uw n;
  s_facts_transaction transaction;
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_3(old, "\"subject\"", ":p", "[1, 2]"));
  TEST_ASSERT(fact_test_init_3(old + 1, "\"subject\"", ":p", "[3, 4]"));
  TEST_ASSERT(fact_test_init_3(&replacement, "\"subject\"", ":p", "[5]"));
  for (n = 0; n < 2; n++) {
    TEST_ASSERT((found = facts_add_fact(&facts, old + n)));
    id[n] = found->id;
  }
  TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
  TEST_ASSERT(facts_replace_fact(&facts, &replacement));
  TEST_EQ(facts_count(&facts), 1);
  TEST_EQ(facts_transaction_rollback(&facts, &transaction), &facts);
  TEST_EQ(facts_count(&facts), 2);
  for (n = 0; n < 2; n++) {
    TEST_EQ(facts_find_fact(&facts, old + n, &found), &found);
    TEST_ASSERT(found);
    TEST_EQ(found->id, id[n]);
    TEST_ASSERT(skiplist_find__fact(facts.index, found));
    TEST_ASSERT(skiplist_find__fact(facts.index_spo, found));
    TEST_ASSERT(skiplist_find__fact(facts.index_pos, found));
    TEST_ASSERT(skiplist_find__fact(facts.index_osp, found));
    fact_test_clean_3(old + n);
  }
  TEST_EQ(facts_find_fact(&facts, &replacement, &found), &found);
  TEST_EQ(found, NULL);
  fact_test_clean_3(&replacement);
  facts_clean(&facts);
}
TEST_CASE_END(facts_transaction_replace)

TEST_CASE(facts_transaction_nested)
{
  bool removed;
  s_fact fact = {0};
  s_fact *found;
  s_facts facts;
  s_facts_transaction inner;
  s_facts_transaction outer;
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_1(&fact, "\"nested\""));
  TEST_EQ(facts_transaction_start(&facts, &outer), &facts);
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  TEST_EQ(facts_transaction_start(&facts, &inner), &facts);
  TEST_EQ(facts_remove_fact(&facts, &fact, &removed), &removed);
  TEST_ASSERT(removed);
  TEST_EQ(facts_transaction_end(&facts, &inner), &facts);
  facts_transaction_clean(&inner);
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  TEST_EQ(facts_transaction_rollback(&facts, &outer), &facts);
  TEST_EQ(facts_count(&facts), 0);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(facts_transaction_start(&facts, &outer), &facts);
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  TEST_EQ(facts_transaction_start(&facts, &inner), &facts);
  TEST_EQ(facts_remove_fact(&facts, &fact, &removed), &removed);
  TEST_EQ(facts_transaction_rollback(&facts, &inner), &facts);
  TEST_EQ(facts.transaction, &outer);
  TEST_EQ(facts_transaction_end(&facts, &outer), &facts);
  TEST_EQ(facts_find_fact(&facts, &fact, &found), &found);
  TEST_ASSERT(found);
  TEST_EQ(found->id, outer.id);
  TEST_EQ(facts_transaction_start(&facts, &outer), &facts);
  TEST_EQ(facts_remove_fact(&facts, &fact, &removed), &removed);
  TEST_EQ(facts_transaction_start(&facts, &inner), &facts);
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  TEST_EQ(facts_transaction_rollback(&facts, &outer), &facts);
  TEST_EQ(facts.transaction, NULL);
  TEST_EQ(facts_count(&facts), 1);
  TEST_EQ(facts.tags.count, 1);
  fact_test_clean_1(&fact);
  facts_clean(&facts);
}
TEST_CASE_END(facts_transaction_nested)

static void facts_test_transaction_hook (void *context,
                                        e_fact_action action,
                                        const s_fact *fact)
{
  uw *events = context;
  (void) fact;
  if (events[0] < 4)
    events[++events[0]] = action;
}

TEST_CASE(facts_transaction_publish)
{
  bool removed;
  s_facts_connection connection = {0};
  uw events[5] = {0};
  s_fact old = {0};
  s_fact added = {0};
  s_facts facts;
  FILE *fp;
  long offset;
  const s_str path = STR("facts_transaction_publish");
  s_facts_transaction inner;
  s_facts_transaction outer;
  s_buf wire;
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_1(&old, "\"old\""));
  TEST_ASSERT(fact_test_init_1(&added, "\"new\""));
  TEST_ASSERT((fp = tmpfile()));
  TEST_ASSERT((facts.log = log_new()));
  TEST_ASSERT(log_open_binary(facts.log, fp, &path));
  TEST_ASSERT(facts_add_fact(&facts, &old));
  TEST_ASSERT(buf_flush(&facts.log->buf) >= 0);
  offset = ftell(fp);
  TEST_ASSERT(offset > 0);
  TEST_ASSERT(log_hook_add(facts.log, facts_test_transaction_hook, events));
  TEST_ASSERT(buf_init_alloc(&wire, 4096));
  TEST_ASSERT(marshall_init(&connection.marshall, 4096));
  connection.buf_rw.w = &wire;
  connection.running = true;
  connection.is_master = true;
  facts.connections = &connection;
  facts.server_count = 1;
  TEST_EQ(facts_transaction_start(&facts, &outer), &facts);
  TEST_ASSERT(facts_add_fact(&facts, &added));
  TEST_EQ(facts_transaction_start(&facts, &inner), &facts);
  TEST_EQ(facts_remove_fact(&facts, &old, &removed), &removed);
  TEST_EQ(facts_transaction_end(&facts, &inner), &facts);
  TEST_EQ(events[0], 0);
  TEST_EQ(wire.wpos, 0);
  TEST_EQ(facts.remove_log, NULL);
  TEST_EQ(ftell(fp), offset);
  TEST_EQ(facts_transaction_rollback(&facts, &outer), &facts);
  TEST_EQ(events[0], 0);
  TEST_EQ(wire.wpos, 0);
  TEST_EQ(facts.remove_log, NULL);
  TEST_EQ(ftell(fp), offset);
  TEST_EQ(facts_transaction_start(&facts, &outer), &facts);
  TEST_ASSERT(facts_add_fact(&facts, &added));
  TEST_EQ(facts_transaction_start(&facts, &inner), &facts);
  TEST_EQ(facts_remove_fact(&facts, &old, &removed), &removed);
  TEST_EQ(facts_transaction_end(&facts, &inner), &facts);
  TEST_EQ(events[0], 0);
  TEST_EQ(facts_transaction_end(&facts, &outer), &facts);
  TEST_EQ(events[0], 2);
  TEST_EQ(events[1], FACT_ACTION_ADD);
  TEST_EQ(events[2], FACT_ACTION_REMOVE);
  TEST_ASSERT(wire.wpos > 0);
  TEST_ASSERT(facts.remove_log);
  TEST_EQ(facts.remove_log->next, NULL);
  TEST_ASSERT(ftell(fp) > offset);
  facts.connections = NULL;
  facts.server_count = 0;
  marshall_clean(&connection.marshall);
  buf_clean(&wire);
  fact_test_clean_1(&old);
  fact_test_clean_1(&added);
  facts_clean(&facts);
  fclose(fp);
}
TEST_CASE_END(facts_transaction_publish)

TEST_CASE(facts_transaction_remote)
{
  bool removed;
  s_facts_connection connection = {0};
  s_fact fact = {0};
  s_facts facts;
  s_facts_transaction transaction = {0};
  s_tag object = {0};
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_1(&fact, "\"remote\""));
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  connection.running = true;
  facts.connections = &connection;
  TEST_EQ(facts_transaction_start(&facts, &transaction), NULL);
  TEST_EQ(facts.transaction, NULL);
  TEST_EQ(facts_replace_fact(&facts, &fact), NULL);
  TEST_ASSERT(tag_init_pvar(&object, &g_sym_Tag));
  TEST_EQ(facts_remove_fact_tags(&facts, fact.subject, fact.predicate,
                                 &object, &removed), NULL);
  TEST_EQ(facts_count(&facts), 1);
  TEST_EQ(facts.transaction, NULL);
#if HAVE_PTHREAD
  TEST_EQ(facts.rwlock.count, 0);
#endif
  facts.connections = NULL;
#ifdef NDEBUG
  facts.next_id = UW_MAX;
  TEST_EQ(facts_transaction_start(&facts, &transaction), NULL);
  TEST_EQ(facts.transaction, NULL);
#if HAVE_PTHREAD
  TEST_EQ(facts.rwlock.count, 0);
#endif
  facts.next_id = 2;
#endif
  TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
  TEST_EQ(facts_transaction_end(&facts, &transaction), &facts);
  tag_clean(&object);
  fact_test_clean_1(&fact);
  facts_clean(&facts);
}
TEST_CASE_END(facts_transaction_remote)

TEST_CASE(facts_transaction_reopen)
{
  bool removed;
  char directory[] = "/tmp/kc3-facts-transaction.XXXXXXXX";
  s_fact old = {0};
  s_fact added = {0};
  s_fact *found;
  s_facts facts;
  uw id;
  uw n;
  s_str path = {0};
  s_facts_transaction transaction;
  TEST_ASSERT(mkdtemp(directory));
  TEST_ASSERT(str_init_f(&path, "%s/store.facts", directory));
  TEST_ASSERT(fact_test_init_1(&old, "\"persisted\""));
  TEST_ASSERT(fact_test_init_1(&added, "\"replacement\""));
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(facts_open_file(&facts, &path) >= 0);
  TEST_ASSERT((found = facts_add_fact(&facts, &old)));
  id = found->id;
  for (n = 0; n < 2; n++) {
    TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
    TEST_EQ(facts_remove_fact(&facts, &old, &removed), &removed);
    TEST_ASSERT(removed);
    TEST_ASSERT(facts_add_fact(&facts, &added));
    if (n)
      TEST_EQ(facts_transaction_end(&facts, &transaction), &facts);
    else
      TEST_EQ(facts_transaction_rollback(&facts, &transaction), &facts);
    facts_clean(&facts);
    TEST_ASSERT(facts_init(&facts));
    TEST_ASSERT(facts_open_file(&facts, &path) >= 0);
    TEST_EQ(facts_count(&facts), 1);
    TEST_EQ(facts_find_fact(&facts, n ? &added : &old, &found), &found);
    TEST_ASSERT(found);
    TEST_EQ(found->id, n ? transaction.id : id);
    TEST_EQ(facts_find_fact(&facts, n ? &old : &added, &found), &found);
    TEST_EQ(found, NULL);
  }
  facts_clean(&facts);
  fact_test_clean_1(&old);
  fact_test_clean_1(&added);
  TEST_EQ(unlink(path.ptr.p_pchar), 0);
  str_clean(&path);
  TEST_EQ(rmdir(directory), 0);
}
TEST_CASE_END(facts_transaction_reopen)

TEST_CASE(facts_transaction_remove_wildcard)
{
  bool removed;
  s_fact fact[2] = {0};
  s_fact *found;
  s_facts facts;
  uw n;
  s_facts_transaction transaction;
  s_tag subject = {0};
  s_tag predicate = {0};
  s_tag object = {0};
  TEST_ASSERT(facts_init(&facts));
  TEST_ASSERT(fact_test_init_1(fact, "\"first\""));
  TEST_ASSERT(fact_test_init_1(fact + 1, "\"second\""));
  TEST_ASSERT(facts_add_fact(&facts, fact));
  TEST_ASSERT(facts_add_fact(&facts, fact + 1));
  TEST_ASSERT(tag_init_pvar(&subject, &g_sym_Tag));
  TEST_ASSERT(tag_init_pvar(&predicate, &g_sym_Tag));
  TEST_ASSERT(tag_init_pvar(&object, &g_sym_Tag));
  TEST_EQ(facts_transaction_start(&facts, &transaction), &facts);
  TEST_EQ(facts_remove_fact_tags(&facts, &subject, &predicate, &object,
                                 &removed), &removed);
  TEST_ASSERT(removed);
  TEST_EQ(facts_count(&facts), 0);
  TEST_EQ(facts_transaction_rollback(&facts, &transaction), &facts);
  TEST_EQ(facts_count(&facts), 2);
  TEST_EQ(facts.tags.count, 2);
  for (n = 0; n < 2; n++) {
    TEST_EQ(facts_find_fact(&facts, fact + n, &found), &found);
    TEST_ASSERT(found);
    fact_test_clean_1(fact + n);
  }
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  facts_clean(&facts);
}
TEST_CASE_END(facts_transaction_remove_wildcard)

TEST_CASE(facts_add)
{
  uw i = 0;
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
  const s_str path = STR("facts_test_dump_file.facts");
  s_fact fact[24];
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  facts_dump_file(&facts, &path);
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
  bool b;
  s_fact *f;
  uw i = 0;
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
  s_facts facts;
  s_fact *pf;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    TEST_EQ(facts_find_fact(&facts, fact + i, &f), &f);
    TEST_ASSERT(! f);
    TEST_EQ(facts_find_fact(&facts, fact + i, &f), &f);
    TEST_ASSERT(! f);
    facts_add_fact(&facts, fact + i);
    TEST_EQ(facts_find_fact(&facts, fact + i, &pf), &pf);
    TEST_ASSERT(pf);
    TEST_EQ(compare_tag(fact[i].subject, pf->subject), 0);
    TEST_EQ(compare_tag(fact[i].predicate, pf->predicate), 0);
    TEST_EQ(compare_tag(fact[i].object, pf->object), 0);
    i++;
  }
  while (i--) {
    facts_remove_fact(&facts, fact + i, &b);
    TEST_EQ(facts_find_fact(&facts, fact + i, &f), &f);
    TEST_ASSERT(! f);
    TEST_EQ(facts_find_fact(&facts, fact + i, &f), &f);
    TEST_ASSERT(! f);
    fact_test_clean_1(fact + i);
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_find)

TEST_CASE(facts_init_clean)
{
  s_facts facts;
  test_context("facts_init, facts_clean");
  TEST_EQ(facts_init(&facts), &facts);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(facts.facts.count, 0);
  TEST_EQ(facts.log, NULL);
  facts_clean(&facts);
  TEST_EQ(facts_init(&facts), &facts);
  TEST_EQ(facts.tags.count, 0);
  TEST_EQ(facts.facts.count, 0);
  TEST_EQ(facts.log, NULL);
  facts_clean(&facts);
  test_context(NULL);
}
TEST_CASE_END(facts_init_clean)

TEST_CASE(facts_load)
{
  s_fact *f;
  uw i = 0;
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
  s_fact fact;
  s_facts facts;
  s_str path;
  facts_init(&facts);
  str_init_1(&path, NULL, "facts_test_load_file.facts");
  TEST_EQ(facts_load_file(&facts, &path), 761);
  TEST_EQ(facts_count(&facts), 23);
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(f);
    fact_test_clean_1(&fact);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_load)

TEST_CASE(facts_log_add)
{
  uw i = 0;
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
  s_facts facts;
  FILE *fp;
  const s_str path = STR("facts_test_log_add.facts");
  fp = fopen("facts_test_log_add.facts", "wb");
  facts_init(&facts);
  facts.log = log_new();
  log_open_binary(facts.log, fp, &path);
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
  bool b;
  uw i = 0;
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
  s_facts facts;
  FILE *fp;
  const s_str path = STR("facts_test_log_remove.facts");
  fp = fopen("facts_test_log_remove.facts", "wb");
  facts_init(&facts);
  facts.log = log_new();
  log_open_binary(facts.log, fp, &path);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  while (i--) {
    facts_remove_fact(&facts, fact + i, &b);
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
  TEST_ASSERT((facts = facts_new()));
  TEST_EQ(facts->tags.count, 0);
  TEST_EQ(facts->facts.count, 0);
  TEST_EQ(facts->log, NULL);
  TEST_EQ(facts->ref_count, 1);
  facts_delete(facts);
}
TEST_CASE_END(facts_new_delete)

TEST_CASE(facts_open_file)
{
  bool b;
  s_fact *f;
  uw i = 0;
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
    "(U8) 0",
    "(U8) 1",
    "(U8) 10",
    "(U16) 0x100",
    "(U32) 0x10000",
    "(U64) 0x100000000",
    "0x10000000000000000",
    "(S8) -1",
    "(S8) -10",
    "(S16) -0x100",
    "(S32) -0x10000",
    "(S64) -0x100000000",
    "-0x10000000000000000",
    NULL
  };
  char *q[24] = {
    "\"b\"",
    ":b",
    "B",
    "b",
    "[[]]",
    "[[[]], []]",
    "{:b, :b}",
    "{{:b, :b}, {:c, :d}}",
    "{b, b}",
    "{{b, b}, {c, d}}",
    "(U8) 2",
    "(U8) 3",
    "(U8) 11",
    "(U16) 0x101",
    "(U32) 0x10001",
    "(U64) 0x100000001",
    "0x10000000000000001",
    "(S8) -2",
    "(S8) -11",
    "(S16) -0x101",
    "(S32) -0x10001",
    "(S64) -0x100000001",
    "-0x10000000000000001",
    NULL
  };
  s_fact fact;
  s_facts facts;
  s_str path;
  if (file_copy("facts_test_open_file.1.in.facts",
                "facts_test_open_file.1.facts")) {
    fprintf(stderr, "%s:%i: %s: file_copy\n", __FILE__, __LINE__,
            __func__);
    exit(1);
  }
  facts_init(&facts);
  str_init_1(&path, NULL, "facts_test_open_file.1.facts");
  TEST_EQ(facts_open_file(&facts, &path), 944);
  TEST_EQ(facts_count(&facts), 23);
  i = 0;
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(f);
    TEST_EQ(facts_remove_fact(&facts, &fact, &b), &b);
    TEST_ASSERT(b);
    fact_test_clean_1(&fact);
    i++;
  }
  i = 0;
  while (q[i]) {
    fact_test_init_1(&fact, q[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(! f);
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
  if (file_copy("facts_test_open_file.2.in.facts",
                "facts_test_open_file.2.facts")) {
    fprintf(stderr, "%s:%i: %s: file_copy", __FILE__, __LINE__,
            __func__);
    exit(1);
  }
  str_init_1(&path, NULL, "facts_test_open_file.2.facts");
  TEST_EQ(facts_open_file(&facts, &path), 1890);
  TEST_EQ(facts_count(&facts), 46);
  i = 0;
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(f);
    fact_test_clean_1(&fact);
    i++;
  }
  i = 0;
  while (q[i]) {
    fact_test_init_1(&fact, q[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(f);
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
  if (file_copy("facts_test_open_file.3.in.facts",
                "facts_test_open_file.3.facts")) {
    fprintf(stderr, "%s:%i: %s: file_copy", __FILE__, __LINE__,
            __func__);
    exit(1);
  }
  str_init_1(&path, NULL, "facts_test_open_file.3.facts");
  TEST_EQ(facts_open_file(&facts, &path), 3878);
  TEST_EQ(facts_count(&facts), 0);
  i = 0;
  while (p[i]) {
    fact_test_init_1(&fact, p[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(! f);
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    fact_test_clean_1(&fact);
    i++;
  }
  i = 0;
  while (q[i]) {
    fact_test_init_1(&fact, q[i]);
    TEST_EQ(facts_find_fact(&facts, &fact, &f), &f);
    TEST_ASSERT(! f);
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

TEST_CASE(facts_open_file_binary_round_trip)
{
  s_facts facts;
  s_str path;
  if (file_copy("facts_test_open_file.2.in.facts",
                "facts_test_open_file_binary_round_trip.facts")) {
    fprintf(stderr, "%s:%i: %s: file_copy\n", __FILE__, __LINE__,
            __func__);
    exit(1);
  }
  str_init_1(&path, NULL,
             "facts_test_open_file_binary_round_trip.facts");
  facts_init(&facts);
  TEST_EQ(facts_open_file(&facts, &path), 1890);
  TEST_EQ(facts_count(&facts), 46);
  facts_close(&facts);
  facts_clean(&facts);
  facts_init(&facts);
  TEST_EQ(facts_open_file(&facts, &path), 46);
  TEST_EQ(facts_count(&facts), 46);
  facts_close(&facts);
  facts_clean(&facts);
  unlink("facts_test_open_file_binary_round_trip.facts");
}
TEST_CASE_END(facts_open_file_binary_round_trip)

TEST_CASE(facts_remove)
{
  bool b;
  uw i = 0;
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
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  while (i--) {
    TEST_EQ(facts_remove_fact(&facts, fact + i, &b), &b);
    TEST_EQ(b, true);
    TEST_EQ(facts.tags.count, i);
    TEST_EQ(facts.facts.count, i);
    /*
    TEST_EQ(facts_remove_fact(&facts, fact + i, &b), &b);
    TEST_EQ(b, false);
    TEST_EQ(facts.tags.count, i);
    TEST_EQ(facts.facts.count, i);
    */
    fact_test_clean_1(fact + i);
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_remove)

TEST_CASE(facts_save)
{
  uw i = 0;
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
  const s_str path = STR("facts_test_save.facts");
  s_fact fact[24];
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  facts_save_file(&facts, &path);
  test_file_compare(path.ptr.p_pchar, "facts_test_save.expected.facts");
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

TEST_CASE(facts_save_binary)
{
  uw i = 0;
  char *p[24] = {
    "\"a\"",
    ":a",
    "A",
    "a",
    "[]",
    "[[], []]",
    "{:a, :b}",
    "{{:a, :b}, {:c, :d}}",
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
  const s_str path = STR("facts_test_save_binary.facts");
  s_fact fact[24];
  s_facts facts;
  facts_init(&facts);
  while (p[i]) {
    fact_test_init_1(fact + i, p[i]);
    facts_add_fact(&facts, fact + i);
    i++;
  }
  facts_save_binary_file(&facts, &path);
  test_file_compare(path.ptr.p_pchar,
                    "facts_test_save_binary.expected.facts");
  if (g_test_last_ok)
    unlink("facts_test_save_binary.facts");
  i = 0;
  while (p[i]) {
    fact_test_clean_1(fact + i);
    i++;
  }
  facts_clean(&facts);
}
TEST_CASE_END(facts_save_binary)

#ifdef NDEBUG
TEST_CASE(facts_close_no_log)
{
  s_facts facts = {0};
  test_context("facts_close() on a facts without a log");
  TEST_EQ(facts_init(&facts), &facts);
  TEST_EQ(facts.log, NULL);
  facts_close(&facts);
  TEST_EQ(facts.log, NULL);
  facts_clean(&facts);
  test_context(NULL);
}
TEST_CASE_END(facts_close_no_log)
#endif
