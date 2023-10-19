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
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "compare.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "list.h"
#include "log.h"
#include "set__fact.h"
#include "set__tag.h"
#include "skiplist__fact.h"
#include "tag.h"

sw facts_open_file_create (s_facts *facts, const s_str *path);
sw facts_open_log (s_facts *facts, s_buf *buf);

s_fact * facts_add_fact (s_facts *facts, const s_fact *fact)
{
  s_fact tmp;
  s_fact *f = NULL;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
  facts_lock_w(facts);
  tmp.subject   = facts_ref_tag(facts, fact->subject);
  tmp.predicate = facts_ref_tag(facts, fact->predicate);
  tmp.object    = facts_ref_tag(facts, fact->object);
  if ((item = set_get__fact(&facts->facts, &tmp))) {
    facts_lock_unlock_w(facts);
    return &item->data;
  }
  tmp.id = facts->next_id;
  if (facts->next_id == UW_MAX) {
    errx(1, "facts serial id exhausted");
    return NULL;
  }
  facts->next_id++;
  if (facts->log)
    facts_log_add(facts->log, &tmp);
  item = set_add__fact(&facts->facts, &tmp);
  f = &item->data;
  skiplist_insert__fact(facts->index_spo, f);
  skiplist_insert__fact(facts->index_pos, f);
  skiplist_insert__fact(facts->index_osp, f);
  facts_lock_unlock_w(facts);
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
  facts_dump_file(facts, "debug.facts");
  if (facts->log)
    facts_close(facts);
  skiplist_delete__fact(facts->index_osp);
  skiplist_delete__fact(facts->index_pos);
  skiplist_delete__fact(facts->index_spo);
  set_clean__fact(&facts->facts);
  set_clean__tag(&facts->tags);
  facts_lock_clean(facts);
}

void facts_close (s_facts *facts)
{
  assert(facts->log);
  log_close(facts->log);
  log_delete(facts->log);
  facts->log = NULL;
}

void facts_delete (s_facts *facts)
{
  assert(facts);
  facts_clean(facts);
  free(facts);
}

sw facts_dump (s_facts *facts, s_buf *buf)
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
  if ((r = buf_write_1(buf,
                       "%{module: C3.Facts.Dump,\n"
                       "  version: 1}\n")) < 0)
    return r;
  result += r;
  facts_lock_r(facts);
  facts_with_0(facts, &cursor, &subject, &predicate, &object);
  while ((fact = facts_cursor_next(&cursor))) {
    if ((r = buf_write_1(buf, "add ")) < 0)
      goto clean;
    result += r;
    if ((r = buf_inspect_fact(buf, fact)) < 0)
      goto clean;
    result += r;
    if ((r = buf_write_1(buf, "\n")) < 0)
      goto clean;
    result += r;
  }
  r = result;
 clean:
  facts_lock_unlock_r(facts);
  return r;
}

sw facts_dump_file (s_facts *facts, const s8 *path)
{
  s8 b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw r;
  assert(facts);
  assert(path);
  buf_init(&buf, false, sizeof(b), b);
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

s_fact * facts_find_fact (s_facts *facts, const s_fact *fact)
{
  s_fact f;
  s_set_item__fact *item;
  s_fact *result = NULL;
  assert(facts);
  assert(fact);
  facts_lock_r(facts);
  if ((f.subject   = facts_find_tag(facts, fact->subject))   &&
      (f.predicate = facts_find_tag(facts, fact->predicate)) &&
      (f.object    = facts_find_tag(facts, fact->object))    &&
      (item = set_get__fact((const s_set__fact *) &facts->facts, &f)))
    result = &item->data;
  facts_lock_unlock_r(facts);
  return result;
}

s_tag * facts_find_tag (s_facts *facts, const s_tag *tag)
{
  s_set_item__tag *item;
  s_tag *result = NULL;
  assert(facts);
  assert(tag);
  facts_lock_r(facts);
  if ((item = set_get__tag(&facts->tags, tag)))
    result = &item->data;
  facts_lock_unlock_r(facts);
  return result;
}

s_facts * facts_init (s_facts *facts)
{
  const u8 max_height = 10;
  const double spacing = 2.7;
  assert(facts);
  set_init__tag(&facts->tags, 1024);
  set_init__fact(&facts->facts, 1024);
  facts->index_spo = skiplist_new__fact(max_height, spacing);
  assert(facts->index_spo);
  facts->index_spo->compare = compare_fact;
  facts->index_pos = skiplist_new__fact(max_height, spacing);
  assert(facts->index_pos);
  facts->index_pos->compare = compare_fact_pos;
  facts->index_osp = skiplist_new__fact(max_height, spacing);
  assert(facts->index_osp);
  facts->index_osp->compare = compare_fact_osp;
  facts->log = NULL;
  facts_lock_init(facts);
  return facts;
}

sw facts_load (s_facts *facts, s_buf *buf, const s_str *path)
{
  s_fact_w fact;
  s_fact *factp;
  u64 line;
  sw r;
  bool replace;
  sw result = 0;
  assert(facts);
  assert(buf);
  if ((r = buf_read_1(buf,
                      "%{module: C3.Facts.Dump,\n"
                      "  version: 1}\n")) <= 0)
    goto ko_header;
  result += r;
  facts_lock_w(facts);
  line = 3;
  while (1) {
    if ((r = buf_read_1(buf, "replace ")) < 0)
      break;
    if (r)
      replace = true;
    else {
      replace = false;
      if ((r = buf_read_1(buf, "add ")) <= 0)
        break;
    }
    result += r;
    if ((r = buf_parse_fact(buf, &fact)) <= 0)
      goto ko_fact;
    result += r;
    factp = fact_r(&fact);
    if (replace)
      facts_replace_fact(facts, factp);
    else
      facts_add_fact(facts, factp);
    fact_w_clean(&fact);
    if ((r = buf_read_1(buf, "\n")) <= 0) {
      r = -1;
      goto ko_fact;
    }
    result += r;
    line++;
  }
  facts_lock_unlock_w(facts);
  return result;
 ko_header:
  warnx("facts_load: %s: invalid or missing header",
        path->ptr.ps8);
  return -1;
 ko_fact:
  facts_lock_unlock_w(facts);
  warnx("facts_load: %s: %s fact line %lu",
        path->ptr.ps8,
        r ? "invalid" : "missing",
        (unsigned long) line);
  return -1;
}

sw facts_load_file (s_facts *facts, const s_str *path)
{
  s8 b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw result;
  assert(facts);
  assert(path);
  buf_init(&buf, false, sizeof(b), b);
  if (! (fp = fopen(path->ptr.ps8, "r"))) {
    warn("facts_load_file: %s", path->ptr.ps8);
    return -1;
  }
  buf_file_open_r(&buf, fp);
  result = facts_load(facts, &buf, path);
  buf_file_close(&buf);
  fclose(fp);
  return result;
}

void facts_lock_clean (s_facts *facts)
{
  assert(facts);
  if (pthread_rwlock_destroy(&facts->rwlock))
    errx(1, "facts_lock_clean: pthread_rwlock_destroy");
}

void facts_lock_init (s_facts *facts)
{
  assert(facts);
  if (pthread_rwlock_init(&facts->rwlock, NULL))
    errx(1, "facts_lock_init: pthread_rwlock_init");
  facts->rwlock_count = 0;
  facts->rwlock_thread = 0;
}

void facts_lock_r (s_facts *facts)
{
  pthread_t thread;
  assert(facts);
  thread = pthread_self();
  if (facts->rwlock_thread == thread)
    return;
  if (pthread_rwlock_rdlock(&facts->rwlock))
    errx(1, "facts_lock_r: pthread_rwlock_rdlock");
}

void facts_lock_unlock_r (s_facts *facts)
{
  pthread_t thread;
  assert(facts);
  thread = pthread_self();
  if (facts->rwlock_thread == thread)
    return;
  if (pthread_rwlock_unlock(&facts->rwlock))
    errx(1, "facts_lock_unlock_r: pthread_rwlock_unlock");
}

void facts_lock_unlock_w (s_facts *facts)
{
  assert(facts);
  facts->rwlock_count--;
  if (! facts->rwlock_count) {
    facts->rwlock_thread = 0;
    if (pthread_rwlock_unlock(&facts->rwlock))
      errx(1, "facts_lock_unlock_w: pthread_rwlock_unlock");
  }
}

void facts_lock_w (s_facts *facts)
{
  pthread_t thread;
  assert(facts);
  thread = pthread_self();
  if (facts->rwlock_thread != thread) {
    if (pthread_rwlock_wrlock(&facts->rwlock))
      errx(1, "facts_lock_w: pthread_rwlock_wrlock");
    facts->rwlock_thread = thread;
    facts->rwlock_count = 0;
  }
  facts->rwlock_count++;
}

sw facts_log_add (s_log *log, const s_fact *fact)
{
  sw r;
  sw result = 0;
  assert(log);
  assert(fact);
  if ((r = buf_write_1(&log->buf, "add ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_fact(&log->buf, fact)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(&log->buf, "\n")) < 0)
    return r;
  result += r;
  return result;
}

sw facts_log_remove (s_log *log, const s_fact *fact)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(&log->buf, "remove ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_fact(&log->buf, fact)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(&log->buf, "\n")) < 0)
    return r;
  result += r;
  return result;
}

s_facts * facts_new ()
{
  s_facts *n;
  if (! (n = malloc(sizeof(s_facts))))
    errx(1, "facts_new: out of memory");
  return facts_init(n);
}

sw facts_open_buf (s_facts *facts, s_buf *buf, const s_str *path)
{
  sw r;
  sw result = 0;
  if ((r = facts_load(facts, buf, path)) <= 0)
    return r;
  result += r;
  if ((r = facts_open_log(facts, buf)) < 0)
    return r;
  result += r;
  return result;
}

sw facts_open_file (s_facts *facts, const s_str *path)
{
  FILE *fp;
  s8 i[BUF_SIZE];
  s_buf in;
  sw r;
  sw result = 0;
  buf_init(&in, false, sizeof(i), i);
  if (! (fp = fopen(path->ptr.ps8, "rb"))) {
    if (errno == ENOENT)
      return facts_open_file_create(facts, path);
    return -1;
  }
  buf_file_open_r(&in, fp);
  if ((r = facts_open_buf(facts, &in, path)) < 0)
    return r;
  result += r;
  buf_file_close(&in);
  if (! (fp = fopen(path->ptr.ps8, "ab"))) {
    warn("facts_open_file: fopen: %s", path->ptr.ps8);
    return -1;
  }
  if (! (facts->log = log_new(BUF_SIZE)))
    return -1;
  log_open(facts->log, fp);
  return result;
}

sw facts_open_file_create (s_facts *facts, const s_str *path)
{
  FILE *fp;
  s_buf *out;
  sw r;
  sw result = 0;
  if (! (fp = fopen(path->ptr.ps8, "wb"))) {
    warn("facts_open_file_create: fopen: %s", path->ptr.ps8);
    return -1;
  }
  if (facts_count(facts)) {
    /* TODO: clear facts
       facts_close(facts);
       facts_remove_all(facts);
    */
    warnx("facts_open_file: not supported");
    return -1;
  }
  out = buf_new_alloc(BUF_SIZE);
  buf_file_open_w(out, fp);
  if ((r = facts_dump(facts, out)) < 0)
    return r;
  result += r;
  buf_flush(out);
  if (! (facts->log = log_new()))
    return -1;
  buf_file_open_w(&facts->log->buf, fp);
  return result;
}

sw facts_open_log (s_facts *facts, s_buf *buf)
{
  s_fact_w fact;
  s_fact *factp;
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
      factp = fact_r(&fact);
      facts_add_fact(facts, factp);
      goto ok;
    }
    if ((r = buf_read_1(buf, "remove ")) <= 0)
      break;
    result += r;
    if ((r = buf_parse_fact(buf, &fact)) <= 0)
      break;
    result += r;
    factp = fact_r(&fact);
    facts_remove_fact(facts, factp);
  ok:
    fact_w_clean(&fact);
    if ((r = buf_read_1(buf, "\n")) <= 0)
      break;
    result += r;
  }
  return result;
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

bool facts_remove_fact (s_facts *facts, const s_fact *fact)
{
  s_fact f;
  s_fact *found;
  e_bool result = false;
  assert(facts);
  assert(fact);
  facts_lock_w(facts);
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
    result = true;
  }
  facts_lock_unlock_w(facts);
  return result;
}


s_fact * facts_replace_fact (s_facts *facts, const s_fact *fact)
{
  assert(facts);
  assert(fact);
  return facts_replace_tags(facts, fact->subject, fact->predicate,
                            fact->object);
}

s_fact * facts_replace_tags (s_facts *facts, const s_tag *subject,
                             const s_tag *predicate,
                             const s_tag *object)
{
  s_facts_cursor cursor;
  s_list *list = NULL;
  s_fact *f;
  s_tag var;
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  tag_init_var(&var);
  facts_lock_w(facts);
  facts_with_tags(facts, &cursor, (s_tag *) subject,
                  (s_tag *) predicate, &var);
  while ((f = facts_cursor_next(&cursor))) {
    list = list_new(NULL, list);
    list->tag.data.fact = *f;
  }
  while (list) {
    facts_remove_fact(facts, &list->tag.data.fact);
    list = list_delete(list);
  }
  facts_cursor_clean(&cursor);
  f = facts_add_tags(facts, subject, predicate, object);
  facts_lock_unlock_w(facts);
  return f;
}

sw facts_save_file (s_facts *facts, const s8 *path)
{
  s8 b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw r;
  sw result = 0;
  assert(facts);
  assert(path);
  assert(! facts->log);
  buf_init(&buf, false, sizeof(b), b);
  if (! (fp = fopen(path, "wb"))) {
    warn("fopen: %s", path);
    return -1;
  }
  buf_file_open_w(&buf, fp);
  if ((r = facts_dump(facts, &buf)) < 0)
    goto ko;
  result += r;
  buf_flush(&buf);
  free(buf.user_ptr);
  buf.user_ptr = NULL;
  if (! (facts->log = log_new()))
    goto ko;
  if (log_open(facts->log, fp) < 0)
    goto ko;
  return result;
 ko:
  fclose(fp);
  return r;
}

bool facts_unref_tag (s_facts *facts, const s_tag *tag)
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
