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
#include <unistd.h>

#include "../libkc3/buf.h"
#include "../libkc3/endian.h"
#include "../libkc3/facts.h"
#include "../libkc3/file.h"
#include "../libkc3/inspect.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/set__fact.h"
#include "../libkc3/set__tag.h"
#include "../libkc3/skiplist__fact.h"
#include "../libkc3/skiplist_node__fact.h"
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "../libkc3/tag_init.h"
#include "test.h"
#include "fact_test.h"
#include "tag_test.h"

#define MARSHALL_READ_TEST(type, test, expected)                        \
  do {                                                                  \
    type tmp = {0};                                                     \
    s_str tmp_str = {{0}, sizeof(expected) - 1, {expected}};            \
    s_marshall_read mr = {0};                                           \
    s_str test_str = {0};                                               \
    test_context("marshall_read_" # type "(" # test ") -> "             \
                 # expected);                                           \
    TEST_EQ(marshall_read_init_1(&mr, (test), sizeof(test) - 1), &mr);  \
    TEST_EQ(marshall_read_## type (&mr, false, &tmp), &mr);             \
    marshall_read_clean(&mr);                                           \
    inspect_ ## type(_Generic(tmp,                                      \
                              f32:  tmp,                                \
                              f64:  tmp,                                \
                              s8:   tmp,                                \
                              s16:  tmp,                                \
                              s32:  tmp,                                \
                              s64:  tmp,                                \
                              u8:   tmp,                                \
                              u16:  tmp,                                \
                              u32:  tmp,                                \
                              u64:  tmp,                                \
                              default: &tmp), &test_str);               \
    TEST_STR_EQ(test_str, tmp_str);                                     \
    str_clean(&test_str);                                               \
    str_clean(&tmp_str);                                                \
    test_context(NULL);                                                 \
  } while (0)

#define MARSHALL_READ_TEST_TAG(test, expected)                          \
  do {                                                                  \
    s_marshall_read mr = {0};                                           \
    s_tag tag = {0};                                                    \
    s_tag tag_expected = {0};                                           \
    test_context("marshall_read_tag(" # test ") -> " # expected);       \
    TEST_ASSERT(tag_init_1(&tag_expected, (expected)));                 \
    TEST_EQ(marshall_read_init_1(&mr, (test), sizeof(test) - 1), &mr);  \
    TEST_EQ(marshall_read_tag(&mr, false, &tag), &mr);                  \
    TAG_TEST_EQ(&tag, &tag_expected);                                   \
    marshall_read_clean(&mr);                                           \
    tag_clean(&tag);                                                    \
    tag_clean(&tag_expected);                                           \
    test_context(NULL);                                                 \
  } while (0)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_read_bool);
TEST_CASE_PROTOTYPE(marshall_read_character);
TEST_CASE_PROTOTYPE(marshall_read_init_buf);
TEST_CASE_PROTOTYPE(marshall_read_init_file);
TEST_CASE_PROTOTYPE(marshall_read_init_str);
TEST_CASE_PROTOTYPE(marshall_read_plist);
TEST_CASE_PROTOTYPE(marshall_read_s8);
TEST_CASE_PROTOTYPE(marshall_read_s16);
TEST_CASE_PROTOTYPE(marshall_read_s32);
TEST_CASE_PROTOTYPE(marshall_read_s64);
TEST_CASE_PROTOTYPE(marshall_read_set_fact);
TEST_CASE_PROTOTYPE(marshall_read_set_tag);
TEST_CASE_PROTOTYPE(marshall_facts_structures_real);
TEST_CASE_PROTOTYPE(marshall_read_facts_structures);
TEST_CASE_PROTOTYPE(marshall_read_skiplist_fact);
TEST_CASE_PROTOTYPE(marshall_read_sw);
TEST_CASE_PROTOTYPE(marshall_read_tag);
TEST_CASE_PROTOTYPE(marshall_read_unquote);
TEST_CASE_PROTOTYPE(marshall_read_u8);
TEST_CASE_PROTOTYPE(marshall_read_u16);
TEST_CASE_PROTOTYPE(marshall_read_u32);
TEST_CASE_PROTOTYPE(marshall_read_u64);
TEST_CASE_PROTOTYPE(marshall_read_uw);

void marshall_read_test (void)
{
  TEST_CASE_RUN(marshall_read_bool);
  TEST_CASE_RUN(marshall_read_tag);
  TEST_CASE_RUN(marshall_read_unquote);
  TEST_CASE_RUN(marshall_read_set_tag);
  TEST_CASE_RUN(marshall_read_set_fact);
  TEST_CASE_RUN(marshall_read_skiplist_fact);
  TEST_CASE_RUN(marshall_read_facts_structures);
  TEST_CASE_RUN(marshall_facts_structures_real);
}

TEST_CASE(marshall_read_bool)
{
  MARSHALL_READ_TEST(bool,
                     "KC3MARSH"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x0a\x00\x00\x00\x00\x00\x00\x00"
                     "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                     "\x5f\x00",
                     "false");
  MARSHALL_READ_TEST(bool,
                     "KC3MARSH"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x0a\x00\x00\x00\x00\x00\x00\x00"
                     "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                     "\x5f\x01",
                     "true");
}
TEST_CASE_END(marshall_read_bool)

  TEST_CASE(marshall_read_plist)
{
  s_marshall_read mr = {0};
  s_tag expected = {0};
  s_tag test_tag = {0};
  const char test_str[] = "KC3MARSH"
    "\x02\x00\x00\x00\x00\x00\x00\x00"
    "\x15\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x10\x00\x10\x00\x00\x00\x00\x00"
    "\x00\x00\x00";
  TEST_EQ(marshall_read_init_1(&mr, test_str, sizeof(test_str) - 1),
          &mr);
  TEST_EQ(marshall_read_plist(&mr, false, &test_tag.data.td_plist), &mr);
  test_tag.type = TAG_PLIST;
  tag_init_plist_1(&expected, "[0, 1]");
  TAG_TEST_EQ(&test_tag, &expected);
  tag_clean(&test_tag);
  tag_clean(&expected);
}
TEST_CASE_END(marshall_read_plist)

TEST_CASE(marshall_read_set_fact)
{
  s_fact fact = {0};
  s_facts facts;
  s_set__fact facts2 = {0};
  uw h;
  uw i;
  const s_set_item__fact *item;
  const s_set_item__fact *item2;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  const char *p[][3] = {
    {"1",      "2",      "3"},
    {"1",      "2",      "4"},
    {":a",     ":b",     ":c"},
    {"\"x\"",  ":is_a",  "\"y\""},
    {"[1, 2]", ":list",  "{3, 4}"},
    {NULL,     NULL,     NULL}
  };
  s_tag object = {0};
  s_tag predicate = {0};
  s_str str = {0};
  s_tag subject = {0};
  s_set_item__tag *tag_item;
  s_set__tag tags2 = {0};
  TEST_EQ(facts_init(&facts), &facts);
  i = 0;
  while (p[i][0]) {
    TEST_ASSERT(fact_test_init_3(&fact, p[i][0], p[i][1], p[i][2]));
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    fact_test_clean_3(&fact);
    i++;
  }
  TEST_EQ(facts.facts.count, i);
  i = 0;
  while (i < 64) {
    tag_init_uw(&subject, i);
    tag_init_uw(&predicate, i + 1000);
    tag_init_uw(&object, i + 2000);
    fact.subject = &subject;
    fact.predicate = &predicate;
    fact.object = &object;
    fact.id = 0;
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    tag_clean(&subject);
    tag_clean(&predicate);
    tag_clean(&object);
    i++;
  }
  TEST_ASSERT(facts.facts.collisions);
  TEST_ASSERT(facts.tags.collisions);
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_set_tag(&m, false, &facts.tags), &m);
  TEST_EQ(marshall_set_fact(&m, false, &facts.facts, &facts.tags), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_set_tag(&mr, false, &tags2), &mr);
  TEST_EQ(marshall_read_set_fact(&mr, false, &facts2, &tags2), &mr);
  TEST_EQ(tags2.count, facts.tags.count);
  TEST_EQ(facts2.max, facts.facts.max);
  TEST_EQ(facts2.count, facts.facts.count);
  TEST_EQ(facts2.collisions, facts.facts.collisions);
  h = 0;
  while (h < facts.facts.max) {
    item = facts.facts.items[h];
    item2 = facts2.items[h];
    while (item && item2) {
      TEST_EQ(item2->hash, item->hash);
      TEST_EQ(item2->data.id, item->data.id);
      FACT_TEST_EQ(&item2->data, &item->data);
      TEST_ASSERT((tag_item = set_get__tag(&tags2, item2->data.subject)));
      TEST_EQ(&tag_item->data, item2->data.subject);
      TEST_ASSERT((tag_item = set_get__tag(&tags2,
                                           item2->data.predicate)));
      TEST_EQ(&tag_item->data, item2->data.predicate);
      TEST_ASSERT((tag_item = set_get__tag(&tags2, item2->data.object)));
      TEST_EQ(&tag_item->data, item2->data.object);
      item = item->next;
      item2 = item2->next;
    }
    TEST_ASSERT(! item);
    TEST_ASSERT(! item2);
    h++;
  }
  marshall_read_clean(&mr);
  marshall_clean(&m);
  set_clean__fact(&facts2);
  set_clean__tag(&tags2);
  str_clean(&str);
  facts_clean(&facts);
}
TEST_CASE_END(marshall_read_set_fact)

#define MARSHALL_FACTS_TEST_BUF_SIZE (64 * 1024 * 1024)

#define MARSHALL_FACTS_TEST_COPY "marshall_facts_real.tmp.facts"


static bool marshall_facts_test_copy (const char *src, const char *dest)
{
  char buf[BUF_SIZE];
  FILE *in;
  FILE *out;
  size_t r;
  if (! (in = fopen(src, "rb")))
    return false;
  if (! (out = fopen(dest, "wb"))) {
    fclose(in);
    return false;
  }
  while ((r = fread(buf, 1, sizeof(buf), in)) > 0)
    if (fwrite(buf, 1, r, out) != r) {
      fclose(in);
      fclose(out);
      return false;
    }
  fclose(in);
  fclose(out);
  return true;
}

static uw marshall_facts_test_remove_unbound (s_facts *facts)
{
  bool b;
  s_fact buf[2048];
  uw count = 0;
  uw h;
  uw i;
  s_set_item__fact *item;
  bool u1 = false;
  bool u2 = false;
  bool u3 = false;
  h = 0;
  while (h < facts->facts.max) {
    item = facts->facts.items[h];
    while (item) {
      if (tag_is_unbound_var(item->data.subject, &u1) &&
          tag_is_unbound_var(item->data.predicate, &u2) &&
          tag_is_unbound_var(item->data.object, &u3) &&
          (u1 || u2 || u3) &&
          count < 2048) {
        buf[count] = item->data;
        count++;
      }
      item = item->next;
    }
    h++;
  }
  i = 0;
  while (i < count) {
    facts_remove_fact(facts, buf + i, &b);
    i++;
  }
  return count;
}

static s_str * marshall_facts_test_to_str (s_marshall *m, s_str *dest)
{
  s_buf out;
  if (! buf_init_alloc(&out, MARSHALL_FACTS_TEST_BUF_SIZE))
    return NULL;
  if (marshall_to_buf(m, &out) <= 0) {
    buf_clean(&out);
    return NULL;
  }
  if (buf_read_to_str(&out, dest) <= 0) {
    buf_clean(&out);
    return NULL;
  }
  buf_clean(&out);
  return dest;
}

TEST_CASE(marshall_facts_structures_real)
{
  s_facts facts;
  s_facts facts_a;
  s_facts facts_b;
  uw h;
  s_set_item__fact *item;
  uw k;
  uw diff_a = 0;
  uw diff_b = 0;
  uw not_found_a = 0;
  uw not_found_b = 0;
  s_marshall m = {0};
  s_marshall m0 = {0};
  s_marshall m2 = {0};
  s_marshall m3 = {0};
  uw removed;
  s_marshall_read mr = {0};
  s_marshall_read mr2 = {0};
  const s_skiplist__fact *index_a[4];
  const s_skiplist__fact *index_b[4];
  const s_skiplist__fact *index_src[4];
  const s_skiplist_node__fact *node_a;
  const s_skiplist_node__fact *node_b;
  const s_skiplist_node__fact *node_src;
  const char *base_path;
  s_str path = {0};
  s_str str = {0};
  s_str str2 = {0};
  s_str str3 = {0};
  if (! (base_path = getenv("KC3_TEST_FACTS_BASE"))) {
    err_puts("\n  marshall_facts_structures_real: KC3_TEST_FACTS_BASE"
             " undefined, skipped");
    return 0;
  }
  TEST_ASSERT(marshall_facts_test_copy(base_path,
                                       MARSHALL_FACTS_TEST_COPY));
  str_init_1(&path, NULL, MARSHALL_FACTS_TEST_COPY);
  TEST_EQ(facts_init(&facts), &facts);
  TEST_ASSERT(facts_open_file(&facts, &path) >= 0);
  facts_close(&facts);
  TEST_ASSERT(facts.facts.count);
  TEST_ASSERT(facts.facts.collisions);
  TEST_EQ(marshall_init(&m0, MARSHALL_FACTS_TEST_BUF_SIZE), &m0);
  TEST_ASSERT(! marshall_facts_structures(&m0, false, &facts));
  marshall_clean(&m0);
  removed = marshall_facts_test_remove_unbound(&facts);
  TEST_ASSERT(removed);
  TEST_EQ(marshall_init(&m, MARSHALL_FACTS_TEST_BUF_SIZE), &m);
  TEST_EQ(marshall_facts(&m, false, &facts), &m);
  TEST_EQ(marshall_facts_test_to_str(&m, &str), &str);
  TEST_EQ(facts_init(&facts_a), &facts_a);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_facts(&mr, false, &facts_a), &mr);
  TEST_EQ(marshall_init(&m2, MARSHALL_FACTS_TEST_BUF_SIZE), &m2);
  TEST_EQ(marshall_facts_structures(&m2, false, &facts), &m2);
  TEST_EQ(marshall_facts_test_to_str(&m2, &str2), &str2);
  TEST_EQ(facts_init(&facts_b), &facts_b);
  TEST_EQ(marshall_read_init_str(&mr2, &str2), &mr2);
  TEST_EQ(marshall_read_facts_structures(&mr2, false, &facts_b), &mr2);
  TEST_EQ(facts_b.facts.count, facts.facts.count);
  TEST_EQ(facts_b.tags.count, facts.tags.count);
  TEST_EQ(facts_b.facts.collisions, facts.facts.collisions);
  TEST_EQ(facts_b.next_id, facts.next_id);
  TEST_EQ(facts_a.facts.count, facts.facts.count);
  TEST_EQ(facts_a.tags.count, facts.tags.count);
  TEST_ASSERT(facts_b.next_id >= facts_a.next_id);
  index_src[0] = facts.index;
  index_src[1] = facts.index_spo;
  index_src[2] = facts.index_pos;
  index_src[3] = facts.index_osp;
  index_a[0] = facts_a.index;
  index_a[1] = facts_a.index_spo;
  index_a[2] = facts_a.index_pos;
  index_a[3] = facts_a.index_osp;
  index_b[0] = facts_b.index;
  index_b[1] = facts_b.index_spo;
  index_b[2] = facts_b.index_pos;
  index_b[3] = facts_b.index_osp;
  k = 0;
  while (k < 4) {
    TEST_EQ(index_b[k]->length, index_src[k]->length);
    TEST_EQ(index_a[k]->length, index_src[k]->length);
    node_src = SKIPLIST_NODE_NEXT__fact(index_src[k]->head, 0);
    node_a = SKIPLIST_NODE_NEXT__fact(index_a[k]->head, 0);
    node_b = SKIPLIST_NODE_NEXT__fact(index_b[k]->head, 0);
    while (node_src && node_a && node_b) {
      if (node_b->height != node_src->height ||
          compare_fact(node_b->fact, node_src->fact))
        diff_b++;
      if (compare_fact(node_a->fact, node_src->fact))
        diff_a++;
      node_src = SKIPLIST_NODE_NEXT__fact(node_src, 0);
      node_a = SKIPLIST_NODE_NEXT__fact(node_a, 0);
      node_b = SKIPLIST_NODE_NEXT__fact(node_b, 0);
    }
    TEST_ASSERT(! node_src);
    TEST_ASSERT(! node_a);
    TEST_ASSERT(! node_b);
    TEST_EQ(diff_b, 0);
    TEST_EQ(diff_a, 0);
    diff_a = 0;
    diff_b = 0;
    k++;
  }
  h = 0;
  while (h < facts_b.facts.max) {
    item = facts_b.facts.items[h];
    while (item) {
      if (! skiplist_find__fact(facts_b.index, &item->data) ||
          ! skiplist_find__fact(facts_b.index_spo, &item->data) ||
          ! skiplist_find__fact(facts_b.index_pos, &item->data) ||
          ! skiplist_find__fact(facts_b.index_osp, &item->data))
        not_found_b++;
      item = item->next;
    }
    h++;
  }
  h = 0;
  while (h < facts_a.facts.max) {
    item = facts_a.facts.items[h];
    while (item) {
      if (! skiplist_find__fact(facts_a.index, &item->data) ||
          ! skiplist_find__fact(facts_a.index_spo, &item->data) ||
          ! skiplist_find__fact(facts_a.index_pos, &item->data) ||
          ! skiplist_find__fact(facts_a.index_osp, &item->data))
        not_found_a++;
      item = item->next;
    }
    h++;
  }
  TEST_EQ(not_found_a, 0);
  TEST_EQ(not_found_b, 0);
  TEST_EQ(marshall_init(&m3, MARSHALL_FACTS_TEST_BUF_SIZE), &m3);
  TEST_EQ(marshall_facts_structures(&m3, false, &facts_b), &m3);
  TEST_EQ(marshall_facts_test_to_str(&m3, &str3), &str3);
  TEST_STR_EQ(str3, str2);
  marshall_read_clean(&mr2);
  marshall_read_clean(&mr);
  marshall_clean(&m3);
  marshall_clean(&m2);
  marshall_clean(&m);
  str_clean(&str3);
  str_clean(&str2);
  str_clean(&str);
  facts_clean(&facts_b);
  facts_clean(&facts_a);
  facts_clean(&facts);
  unlink(MARSHALL_FACTS_TEST_COPY);
}
TEST_CASE_END(marshall_facts_structures_real)

TEST_CASE(marshall_read_facts_structures)
{
  s_fact fact = {0};
  s_facts facts;
  s_facts facts_a;
  s_facts facts_b;
  uw h;
  uw i;
  uw k;
  s_marshall m = {0};
  s_marshall m2 = {0};
  s_marshall_read mr = {0};
  s_marshall_read mr2 = {0};
  const s_skiplist_node__fact *node_a;
  const s_skiplist_node__fact *node_b;
  const s_skiplist__fact *index_a[4];
  const s_skiplist__fact *index_b[4];
  s_tag object = {0};
  s_tag predicate = {0};
  s_str str = {0};
  s_str str2 = {0};
  s_tag subject = {0};
  s_set_item__tag *tag_item;
  s_set_item__tag *tag_item_b;
  TEST_EQ(facts_init(&facts), &facts);
  i = 0;
  while (i < 64) {
    tag_init_uw(&subject, i % 8);
    tag_init_uw(&predicate, i + 1000);
    tag_init_uw(&object, i + 2000);
    fact.subject = &subject;
    fact.predicate = &predicate;
    fact.object = &object;
    fact.id = 0;
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    tag_clean(&subject);
    tag_clean(&predicate);
    tag_clean(&object);
    i++;
  }
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_facts(&m, false, &facts), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(facts_init(&facts_a), &facts_a);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_facts(&mr, false, &facts_a), &mr);
  TEST_EQ(marshall_init(&m2, BUF_SIZE), &m2);
  TEST_EQ(marshall_facts_structures(&m2, false, &facts), &m2);
  TEST_EQ(marshall_to_str(&m2, &str2), &str2);
  TEST_EQ(facts_init(&facts_b), &facts_b);
  TEST_EQ(marshall_read_init_str(&mr2, &str2), &mr2);
  TEST_EQ(marshall_read_facts_structures(&mr2, false, &facts_b), &mr2);
  TEST_EQ(facts_b.facts.count, facts_a.facts.count);
  TEST_EQ(facts_b.tags.count, facts_a.tags.count);
  TEST_ASSERT(facts_b.next_id >= facts_a.next_id);
  index_a[0] = facts_a.index;
  index_a[1] = facts_a.index_spo;
  index_a[2] = facts_a.index_pos;
  index_a[3] = facts_a.index_osp;
  index_b[0] = facts_b.index;
  index_b[1] = facts_b.index_spo;
  index_b[2] = facts_b.index_pos;
  index_b[3] = facts_b.index_osp;
  k = 0;
  while (k < 4) {
    TEST_EQ(index_b[k]->length, index_a[k]->length);
    node_a = SKIPLIST_NODE_NEXT__fact(index_a[k]->head, 0);
    node_b = SKIPLIST_NODE_NEXT__fact(index_b[k]->head, 0);
    while (node_a && node_b) {
      FACT_TEST_EQ(node_b->fact, node_a->fact);
      TEST_EQ(node_b->fact->id, node_a->fact->id);
      node_a = SKIPLIST_NODE_NEXT__fact(node_a, 0);
      node_b = SKIPLIST_NODE_NEXT__fact(node_b, 0);
    }
    TEST_ASSERT(! node_a);
    TEST_ASSERT(! node_b);
    k++;
  }
  h = 0;
  while (h < facts_a.tags.max) {
    tag_item = facts_a.tags.items[h];
    while (tag_item) {
      TEST_ASSERT((tag_item_b = set_get__tag(&facts_b.tags,
                                             &tag_item->data)));
      TEST_EQ(tag_item_b->usage, tag_item->usage);
      tag_item = tag_item->next;
    }
    h++;
  }
  marshall_read_clean(&mr2);
  marshall_read_clean(&mr);
  marshall_clean(&m2);
  marshall_clean(&m);
  str_clean(&str2);
  str_clean(&str);
  facts_clean(&facts_b);
  facts_clean(&facts_a);
  facts_clean(&facts);
}
TEST_CASE_END(marshall_read_facts_structures)

TEST_CASE(marshall_read_skiplist_fact)
{
  s_fact fact = {0};
  s_set_item__fact *fact_item;
  s_facts facts;
  s_set__fact facts2 = {0};
  const t_skiplist_height *height_table;
  const t_skiplist_height *height_table2;
  uw i;
  u8 level;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  const s_skiplist_node__fact *node;
  const s_skiplist_node__fact *node2;
  s_tag object = {0};
  s_tag predicate = {0};
  s_skiplist__fact *skiplist2 = NULL;
  s_str str = {0};
  s_tag subject = {0};
  s_set__tag tags2 = {0};
  TEST_EQ(facts_init(&facts), &facts);
  i = 0;
  while (i < 64) {
    tag_init_uw(&subject, i);
    tag_init_uw(&predicate, i + 1000);
    tag_init_uw(&object, i + 2000);
    fact.subject = &subject;
    fact.predicate = &predicate;
    fact.object = &object;
    fact.id = 0;
    TEST_ASSERT(facts_add_fact(&facts, &fact));
    tag_clean(&subject);
    tag_clean(&predicate);
    tag_clean(&object);
    i++;
  }
  TEST_EQ(facts.index_spo->length, facts.facts.count);
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_set_tag(&m, false, &facts.tags), &m);
  TEST_EQ(marshall_set_fact(&m, false, &facts.facts, &facts.tags), &m);
  TEST_EQ(marshall_skiplist_fact(&m, false, facts.index_spo,
                                 &facts.facts), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_set_tag(&mr, false, &tags2), &mr);
  TEST_EQ(marshall_read_set_fact(&mr, false, &facts2, &tags2), &mr);
  TEST_EQ(marshall_read_skiplist_fact(&mr, false, &skiplist2, &facts2,
                                      compare_fact_spo), &mr);
  TEST_ASSERT(skiplist2);
  TEST_ASSERT(skiplist2->compare == compare_fact_spo);
  TEST_EQ(skiplist2->max_height, facts.index_spo->max_height);
  TEST_EQ(skiplist2->length, facts.index_spo->length);
  height_table = SKIPLIST_HEIGHT_TABLE__fact(facts.index_spo);
  height_table2 = SKIPLIST_HEIGHT_TABLE__fact(skiplist2);
  i = 0;
  while (i < skiplist2->max_height) {
    TEST_EQ(height_table2[i], height_table[i]);
    i++;
  }
  level = 0;
  while (level < facts.index_spo->max_height) {
    node = SKIPLIST_NODE_NEXT__fact(facts.index_spo->head, level);
    node2 = SKIPLIST_NODE_NEXT__fact(skiplist2->head, level);
    while (node && node2) {
      TEST_EQ(node2->height, node->height);
      FACT_TEST_EQ(node2->fact, node->fact);
      TEST_ASSERT((fact_item = set_get__fact(&facts2, node2->fact)));
      TEST_EQ(&fact_item->data, node2->fact);
      node = SKIPLIST_NODE_NEXT__fact(node, level);
      node2 = SKIPLIST_NODE_NEXT__fact(node2, level);
    }
    TEST_ASSERT(! node);
    TEST_ASSERT(! node2);
    level++;
  }
  marshall_read_clean(&mr);
  marshall_clean(&m);
  skiplist_delete__fact(skiplist2);
  set_clean__fact(&facts2);
  set_clean__tag(&tags2);
  str_clean(&str);
  facts_clean(&facts);
}
TEST_CASE_END(marshall_read_skiplist_fact)

TEST_CASE(marshall_read_set_tag)
{
  uw h;
  uw i;
  const s_set_item__tag *item;
  const s_set_item__tag *item2;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  const char *p[] = {
    "false",
    "true",
    "'a'",
    "\"abc\"",
    ":a",
    "A.b(c, d)",
    "[1, 2, 3]",
    "{:a, :b}",
    "0x10000000000000000",
    "-1",
    NULL
  };
  s_set__tag set = {0};
  s_set__tag set2 = {0};
  s_set_item__tag *set_item;
  s_str str = {0};
  s_tag tag = {0};
  TEST_EQ(set_init__tag(&set, 8), &set);
  tag_init(&tag);
  i = 0;
  while (p[i]) {
    TEST_ASSERT(tag_1(&tag, p[i]));
    TEST_ASSERT((set_item = set_add__tag(&set, &tag)));
    set_item->usage = i + 1;
    i++;
  }
  tag_clean(&tag);
  TEST_EQ(set.count, i);
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_set_tag(&m, false, &set), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_set_tag(&mr, false, &set2), &mr);
  TEST_EQ(set2.max, set.max);
  TEST_EQ(set2.count, set.count);
  TEST_EQ(set2.collisions, set.collisions);
  h = 0;
  while (h < set.max) {
    item = set.items[h];
    item2 = set2.items[h];
    while (item && item2) {
      TEST_EQ(item2->hash, item->hash);
      TEST_EQ(item2->usage, item->usage);
      TAG_TEST_EQ(&item2->data, &item->data);
      item = item->next;
      item2 = item2->next;
    }
    TEST_ASSERT(! item);
    TEST_ASSERT(! item2);
    h++;
  }
  marshall_read_clean(&mr);
  marshall_clean(&m);
  set_clean__tag(&set2);
  set_clean__tag(&set);
  str_clean(&str);
}
TEST_CASE_END(marshall_read_set_tag)

  TEST_CASE(marshall_read_tag)
{
  // u8
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x00",
                         "0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x01",
                         "1");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x02",
                         "2");
  // u16
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1a\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x12"
                         "\x5f\x4b\x43\x33\x55\x31\x36\x5f"
                         "\x00\x01",
                         "256");
  // u32
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x13"
                         "\x5f\x4b\x43\x33\x55\x33\x32\x5f"
                         "\x00\x00\x01\x00",
                         "65536");
  // u64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x14"
                         "\x5f\x4b\x43\x33\x55\x36\x34\x5f"
                         "\xA3\x8D#\xD6\xE2S\x00\x00",
                         "92233720368547");
  // s8:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x10"
                         "\x5f\x4b\x43\x33\x53\x38\x5f\xff",
                         "-1");
  // s16:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1a\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0f"
                         "\x5f\x4b\x43\x33\x53\x31\x36\x5f"
                         "\x7f\xff",
                         "-129");
  // s32:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0e"
                         "\x5f\x4b\x43\x33\x53\x33\x32\x5f"
                         "\x00\x00\x00\x80",
                         "-2147483648");
  // s64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0d"
                         "\x5f\x4b\x43\x33\x53\x36\x34\x5f"
                         "\x5d\xf2\xa6\x63\x7e\xd0\xff\xff",
                         "-52233720368547");
  // f32
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\xdb\x0f\x49\x40",
                         "(F32) 3.1415927");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\x00\x00\x80\x3F",
                         "(F32) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\xFF\xFF\xFF\xFF",
                         "(F32) 4294967295.0");
  // f64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x18\x2d\x44\x54\xfb\x21\x09\x40",
                         "(F64) 3.141592653589793");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\xF0\x3F",
                         "(F64) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\xF0\x41",
                         "(F64) 4294967296.0");
#if HAVE_F80
  // f80
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 0.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x80\xff\x3f\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x5f\x32\xc2\x68\x21\xa2\xda\x0f"
                         "\xc9\x00\x40\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 3.1415926535897932384"
                         "28841971");
#endif
  // str
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0D\x00\x00\x00\x00\x00\x00\x00"
                         "\x23\x0B\x68\x65\x6C\x6C\x6F\x20"
                         "\x77\x6F\x72\x6C\x64",
                         "\"hello world\"");
  */

  // list
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x16\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x11\x02\x16\x2e\x00"
                         "\x00\x00\x00\x00\x00\x00\x16\x20"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "[1, 2]");
  */
  // Do block
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x10\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x03\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x11\x01\x11\x02\x11\x03",
                         "do\n"
                         "  1\n"
                         "  2\n"
                         "  3\n"
                         "end");
  */
  // Tuple
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x03\x16\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x11\x04\x16\x2e\x00"
                         "\x00\x00\x00\x00\x00\x00\x25\x03"
                         "\x00\x00\x00\x00\x00\x00\x00\x11"
                         "\x01\x11\x02\x16\x20\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "{1, 2, [3, 4]}");
  */
  // psym
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x1d\x01\x06\x00\x00\x00\x4d\x6f"
                         "\x64\x75\x6c\x65",
                         "Module");
  */
  // Map
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1B\x00\x00\x00\x00\x00\x00\x00"
                         "\x17\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x1D\x01\x01\x00\x00\x00\x61"
                         "\x11\x01\x1D\x01\x01\x00\x00\x00"
                         "\x62\x11\x02", "%{a: 1, b: 2}");
  */
  // Cfn
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x07\x00\x00\x00\x00\x00\x00\x00"
                         "\x6b\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x21\x00\x00\x00\x00\x00\x00"
                         "\x00\x29\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x33\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x03\x00\x00\x00"
                         "\x54\x61\x67\x43\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x07\x00\x00\x00"
                         "\x74\x61\x67\x5f\x61\x64\x64\x03"
                         "\x58\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x03\x00\x00\x00\x54\x61\x67"
                         "\x68\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x03\x00\x00\x00\x54\x61\x67"
                         "\x78\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x06\x00\x00\x00\x52\x65\x73"
                         "\x75\x6c\x74\x18\x20\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "cfn Tag \"tag_add\" (Tag, Tag, Result)");
  */
  // Call
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0e\x00\x00\x00\x00\x00\x00\x00"
                         "\x35\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x09\x00\x00\x00\x64\x65\x66"
                         "\x6d\x6f\x64\x75\x6c\x65\x04\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x02"
                         "\x00\x00\x00\x00\x00\x00\x00\x1d"
                         "\x01\x04\x00\x00\x00\x54\x65\x73"
                         "\x74\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x2e\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "defmodule Test do end");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x15\x00\x00\x00\x00\x00\x00\x00"
                         "\x2a\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x04\x00\x00\x00\x4c\x69\x73"
                         "\x74\x01\x07\x00\x00\x00\x72\x65"
                         "\x76\x65\x72\x73\x65\x04\x20\x00"
                         "\x00\x00\x00\x00\x00\x00\x29\x00"
                         "\x00\x00\x00\x00\x00\x00\x01\x00"
                         "\x00\x00\x00\x00\x00\x00\x16\x35"
                         "\x00\x00\x00\x00\x00\x00\x00\x35"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "List.reverse([])");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x34\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x3d\x01\x01"
                         "\x00\x00\x00\x61\x04\x20\x00\x00"
                         "\x00\x00\x00\x00\x00\x20\x00\x00"
                         "\x00\x00\x00\x00\x00\x02\x00\x00"
                         "\x00\x00\x00\x00\x00\x27\x26\x00"
                         "\x00\x00\x00\x00\x00\x00\x26\x00"
                         "\x00\x00\x00\x00\x00\x00\x11\x01"
                         "\x2c\x00\x00\x00\x00\x00\x00\x00",
                         "a = 1");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x0b\x00\x00\x00\x00\x00\x00\x00"
                         "\x84\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x2b\x00\x00\x00\x00"
                         "\x00\x00\x00\x33\x00\x00\x00\x00"
                         "\x00\x00\x00\x27\x3c\x00\x00\x00"
                         "\x00\x00\x00\x00\x44\x00\x00\x00"
                         "\x00\x00\x00\x00\x01\x01\x00\x00"
                         "\x00\x78\x16\x53\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x04\x65\x00\x00"
                         "\x00\x00\x00\x00\x00\x6d\x00\x00"
                         "\x00\x00\x00\x00\x00\x01\x01\x00"
                         "\x00\x00\x2a\x02\x00\x00\x00\x00"
                         "\x00\x00\x00\x27\x84\x00\x00\x00"
                         "\x00\x00\x00\x00\x8c\x00\x00\x00"
                         "\x00\x00\x00\x00\x01\x01\x00\x00"
                         "\x00\x78\x11\x02\x9c\x00\x00\x00"
                         "\x00\x00\x00\x00\x18\x20\x00\x00"
                         "\x00\x00\x00\x00\x00",
                         "fn (x) { x * 2 }");
  */
  // Ratio
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x15\x00\x00\x00\x00\x00\x00\x00"
                         "\x0b\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x02\x02\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x06",
                         "2/6");
  */
  // Quote
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x06\x00\x00\x00\x00\x00\x00\x00"
                         "\x26\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x2b\x22\x04"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x11\x01\x26\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "quote 1 + 1");
  */
  // Var
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x01\x02\x00\x00\x00\x55\x38"
                         "\x00\x21\x20\x00\x00\x00\x00\x00"
                         "\x00\x00",
                         "(U8) ?");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0a\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x01\x03\x00\x00\x00\x54\x61"
                         "\x67\x00\x21\x20\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "(Tag) ?");
  // Complex
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x07\x00\x00\x00\x00\x00\x00\x00"
                         "\x25\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x02\x00\x00\x00\x2b\x69\x04"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x11\x02\x27\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "1 +i 2");
  // Unquote
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x00"
                         "\x26\x11\x7b",
                         "unquote(123)");
  */
  // Time
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x48\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x27"
                         "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                         "\x5f\x00\x5f\x4b\x43\x33\x53\x57"
                         "\x5f\x5f\x4b\x43\x33\x53\x36\x34"
                         "\x5f\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x5f\x4b\x43\x33\x53\x57\x5f"
                         "\x5f\x4b\x43\x33\x53\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00",
                         "%Time{}");
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x12\x00\x00\x00\x00\x00\x00\x00"
                         "\x24\x00\xe8\x03\x00\x00\x00\x00"
                         "\x00\x00\xd0\x07\x00\x00\x00\x00"
                         "\x00\x00",
                         "%Time{tv_sec: 1000, tv_nsec: 2000}");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x4c\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x2b\x01\x01"
                         "\x00\x00\x00\x2b\x24\x01\x04\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x02"
                         "\x00\x00\x00\x00\x00\x00\x00\x11"
                         "\x01\x11\x01\x26\x00\x00\x00\x00"
                         "\x00\x00\x00\x04\x26\x00\x00\x00"
                         "\x00\x00\x00\x00\x26\x00\x00\x00"
                         "\x00\x00\x00\x00\x02\x00\x00\x00"
                         "\x00\x00\x00\x00\x11\x02\x11\x02"
                         "\x2c\x00\x00\x00\x00\x00\x00\x00",
                         "%Time{tv_sec: 1 + 1, tv_nsec: 2 + 2}");
  */
  // Ptr
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x2f\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x22"
                         "\x5f\x4b\x43\x33\x50\x54\x52\x5f"
                         "\x5f\x4b\x43\x33\x55\x57\x5f\x5f"
                         "\x4b\x43\x33\x55\x36\x34\x5f\x00"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "(Ptr) 0");
  // PtrFree
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x33\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x23"
                         "\x5f\x4b\x43\x33\x50\x54\x52\x46"
                         "\x52\x45\x45\x5f\x5f\x4b\x43\x33"
                         "\x55\x57\x5f\x5f\x4b\x43\x33\x55"
                         "\x36\x34\x5f\x00\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "(PtrFree) 0");

}
TEST_CASE_END(marshall_read_tag)

TEST_CASE(marshall_read_unquote)
{
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_str str = {0};
  s_tag tag = {0};
  s_tag tag_read = {0};
  test_context("marshall_read unquote(123) roundtrip");
  TEST_ASSERT(tag_init_1(&tag, "unquote 123"));
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_tag(&m, false, &tag), &m);
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_tag(&mr, false, &tag_read), &mr);
  TAG_TEST_EQ(&tag_read, &tag);
  marshall_clean(&m);
  marshall_read_clean(&mr);
  str_clean(&str);
  tag_clean(&tag);
  tag_clean(&tag_read);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_unquote)
