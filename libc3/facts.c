/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "set__fact.h"
#include "set__tag.h"
#include "skiplist__fact.h"
#include "tag.h"

s_fact * facts_add_fact (s_facts *facts, const s_fact *fact)
{
  s_fact tmp;
  s_fact *f = NULL;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
  tmp.subject   = facts_ref_tag(facts, fact->subject);
  tmp.predicate = facts_ref_tag(facts, fact->predicate);
  tmp.object    = facts_ref_tag(facts, fact->object);
  if ((item = set_get__fact(&facts->facts, &tmp)))
    return &item->data;
  if (facts->log)
    facts_log_add(facts->log, &tmp);
  item = set_add__fact(&facts->facts, &tmp);
  f = &item->data;
  skiplist_insert__fact(facts->index_spo, f);
  skiplist_insert__fact(facts->index_pos, f);
  skiplist_insert__fact(facts->index_osp, f);
  return f;
}

s_fact * facts_add_tags (s_facts *facts, const s_tag *subject,
                         const s_tag *predicate,
                         const s_tag *object)
{
  s_fact fact;
  fact_init(&fact, subject, predicate, object);
  return facts_add_fact(facts, &fact);
}

void facts_clean (s_facts *facts)
{
  if (facts->log)
    buf_file_close(facts->log);
  skiplist_delete__fact(facts->index_osp);
  skiplist_delete__fact(facts->index_pos);
  skiplist_delete__fact(facts->index_spo);
  set_clean__fact(&facts->facts);
  set_clean__tag(&facts->tags);
}

void facts_delete (s_facts *facts)
{
  assert(facts);
  facts_clean(facts);
  free(facts);
}

sw facts_dump (const s_facts *facts, s_buf *buf)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag predicate;
  s_tag object;
  sw r;
  sw result = 0;
  s_tag subject;
  assert(facts);
  assert(buf);
  tag_init_var(&subject);
  tag_init_var(&predicate);
  tag_init_var(&object);
  facts_with_0(facts, &cursor, &subject, &predicate, &object);
  while ((fact = facts_cursor_next(&cursor))) {
    if ((r = facts_log_add(buf, fact)) < 0)
      return r;
    result += r;
  }
  return result;
}

sw facts_dump_file (const s_facts *facts, const s8 *path)
{
  s_buf buf;
  FILE *fp;
  sw r;
  assert(facts);
  assert(path);
  BUF_INIT_ALLOCA(&buf, 1024);
  if (! (fp = fopen(path, "wb"))) {
    warn("fopen: %s", path);
    return -1;
  }
  buf_file_open_w(&buf, fp);
  r = facts_dump(facts, &buf);
  buf_file_close(&buf);
  fclose(fp);
  return r;
}

s_fact * facts_find_fact (const s_facts *facts, const s_fact *fact)
{
  s_fact f;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
  if ((f.subject   = facts_find_tag(facts, fact->subject))   &&
      (f.predicate = facts_find_tag(facts, fact->predicate)) &&
      (f.object    = facts_find_tag(facts, fact->object))    &&
      (item = set_get__fact((const s_set__fact *) &facts->facts, &f)))
    return &item->data;
  return NULL;
}

s_tag * facts_find_tag (const s_facts *facts, const s_tag *tag)
{
  s_set_item__tag *item;
  assert(facts);
  assert(tag);
  if ((item = set_get__tag(&facts->tags, tag)))
    return &item->data;
  return NULL;
}

s_facts * facts_init (s_facts *facts, s_buf *log)
{
  const u8 max_height = 10;
  const double spacing = 2.7;
  assert(facts);
  set_init__tag(&facts->tags, 1024);
  set_init__fact(&facts->facts, 1024);
  facts->index_spo = skiplist_new__fact(max_height, spacing);
  assert(facts->index_spo);
  facts->index_spo->compare = fact_compare;
  facts->index_pos = skiplist_new__fact(max_height, spacing);
  assert(facts->index_pos);
  facts->index_pos->compare = fact_compare_pos;
  facts->index_osp = skiplist_new__fact(max_height, spacing);
  assert(facts->index_osp);
  facts->index_osp->compare = fact_compare_osp;
  facts->log = log;
  return facts;
}

sw facts_load (s_facts *facts, s_buf *buf)
{
  s_fact fact;
  sw r;
  sw result = 0;
  assert(facts);
  assert(buf);
  while (1) {
    if ((r = buf_read_1(buf, "add ")) < 0)
      break;
    result += r;
    if (r) {
      if ((r = buf_parse_fact(buf, &fact)) <= 0)
        break;
      result += r;
      facts_add_fact(facts, &fact);
      goto ok;
    }
    if ((r = buf_read_1(buf, "remove ")) <= 0)
      break;
    result += r;
    if ((r = buf_parse_fact(buf, &fact)) <= 0)
      break;
    result += r;
    facts_remove_fact(facts, &fact);
  ok:
    buf_read_1(buf, "\n");
  }
  return result;
}

sw facts_load_file (s_facts *facts, const s8 *path)
{
  s_buf buf;
  FILE *fp;
  sw result;
  assert(facts);
  assert(path);
  BUF_INIT_ALLOCA(&buf, 1024);
  fp = fopen(path, "r");
  buf_file_open_r(&buf, fp);
  result = facts_load(facts, &buf);
  buf_file_close(&buf);
  fclose(fp);
  return result;
}

sw facts_log_add (s_buf *log, const s_fact *fact)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(log, "add ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_fact(log, fact)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(log, "\n")) < 0)
    return r;
  result += r;
  return result;
}

sw facts_log_remove (s_buf *log, const s_fact *fact)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(log, "remove ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_fact(log, fact)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(log, "\n")) < 0)
    return r;
  result += r;
  return result;
}

s_facts * facts_new (s_buf *log)
{
  s_facts *n;
  if (! (n = malloc(sizeof(s_facts))))
    errx(1, "facts_new: out of memory");
  return facts_init(n, log);
}

s_tag * facts_ref_tag (s_facts *facts, const s_tag *tag)
{
  s_set_item__tag *item;
  assert(facts);
  assert(tag);
  item = set_add__tag(&facts->tags, tag);
  item->usage++;
  return &item->data;
}

e_bool facts_remove_fact (s_facts *facts, const s_fact *fact)
{
  s_fact f;
  s_fact *found;
  assert(facts);
  assert(fact);
  found = facts_find_fact(facts, fact);
  if (found) {
    if (facts->log)
      facts_log_remove(facts->log, found);
    skiplist_remove__fact(facts->index_spo, found);
    skiplist_remove__fact(facts->index_pos, found);
    skiplist_remove__fact(facts->index_osp, found);
    f = *found;
    set_remove__fact(&facts->facts, found);
    facts_unref_tag(facts, f.subject);
    facts_unref_tag(facts, f.predicate);
    facts_unref_tag(facts, f.object);
    return true;
  }
  return false;
}

e_bool facts_unref_tag (s_facts *facts, const s_tag *tag)
{
  s_set_item__tag *item;
  assert(facts);
  assert(tag);
  item = set_get__tag(&facts->tags, tag);
  if (item) {
    item->usage--;
    if (! item->usage)
      set_remove_item__tag(&facts->tags, item);
    return true;
  }
  return false;
}
