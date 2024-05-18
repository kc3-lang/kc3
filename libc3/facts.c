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
#include <errno.h>
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "compare.h"
#include "fact.h"
#include "fact_list.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "file.h"
#include "io.h"
#include "list.h"
#include "log.h"
#include "set__fact.h"
#include "set__tag.h"
#include "set_cursor__fact.h"
#include "skiplist__fact.h"
#include "tag.h"

static int facts_compare_pfact_id_reverse (const void *a,
                                           const void *b);
static sw facts_open_file_create (s_facts *facts, const s_str *path);
static sw facts_open_log (s_facts *facts, s_buf *buf);

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
  tmp.id        = 0;
  if ((item = set_get__fact(&facts->facts, &tmp))) {
    facts_lock_unlock_w(facts);
    return &item->data;
  }
  tmp.id = facts->next_id;
  if (facts->next_id == UW_MAX) {
    err_puts("facts_add_fact: facts serial id exhausted");
    assert(! "facts_add_fact: facts serial id exhausted");
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
  if (facts->log)
    facts_close(facts);
  facts_remove_all(facts);
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

int facts_compare_pfact_id_reverse (const void *a, const void *b)
{
  const s_fact *fa;
  const s_fact *fb;
  const s_fact * const *pfa;
  const s_fact * const *pfb;
  assert(a);
  assert(b);
  pfa = a;
  pfb = b;
  fa = *pfa;
  fb = *pfb;
  if (fa->id < fb->id)
    return 1;
  if (fa->id > fb->id)
    return -1;
  return 0;
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

sw facts_dump_file (s_facts *facts, const char *path)
{
  char b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw r;
  assert(facts);
  assert(path);
  buf_init(&buf, false, sizeof(b), b);
  fp = file_open(path, "wb");
  if (! fp)
    return -1;
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

s_fact * facts_find_fact_by_tags (s_facts *facts, const s_tag *subject,
                                  const s_tag *predicate,
                                  const s_tag *object)
{
  s_fact f = {subject, predicate, object, 0};
  return facts_find_fact(facts, &f);
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
  facts->next_id = 1;
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
  err_write_1("facts_load: invalid or missing header: ");
  err_puts(path->ptr.pchar);
  return -1;
 ko_fact:
  facts_lock_unlock_w(facts);
  err_write_1("facts_load: ");
  err_write_1(r ? "invalid" : "missing");
  err_write_1(" fact line ");
  err_inspect_u64(&line);
  err_write_1(": ");
  err_puts(path->ptr.pchar);
  return -1;
}

sw facts_load_file (s_facts *facts, const s_str *path)
{
  char b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw result;
  assert(facts);
  assert(path);
  buf_init(&buf, false, sizeof(b), b);
  fp = file_open(path->ptr.pchar, "rb");
  if (! fp)
    return -1;
  buf_file_open_r(&buf, fp);
  result = facts_load(facts, &buf, path);
  buf_file_close(&buf);
  fclose(fp);
  return result;
}

s_facts * facts_lock_clean (s_facts *facts)
{
  assert(facts);
  if (pthread_rwlock_destroy(&facts->rwlock)) {
    err_puts("facts_lock_clean: pthread_rwlock_destroy");
    assert(! "facts_lock_clean: pthread_rwlock_destroy");
    return NULL;
  }
  return facts;
}

s_facts * facts_lock_init (s_facts *facts)
{
  assert(facts);
  if (pthread_rwlock_init(&facts->rwlock, NULL)) {
    err_puts("facts_lock_init: pthread_rwlock_init");
    assert(! "facts_lock_init: pthread_rwlock_init");
    return NULL;
  }
  facts->rwlock_count = 0;
  facts->rwlock_thread = 0;
  return facts;
}

s_facts * facts_lock_r (s_facts *facts)
{
  pthread_t thread;
  assert(facts);
  thread = pthread_self();
  if (facts->rwlock_thread != thread &&
      pthread_rwlock_rdlock(&facts->rwlock)) {
    err_puts("facts_lock_r: pthread_rwlock_rdlock");
    assert(! "facts_lock_r: pthread_rwlock_rdlock");
    return NULL;
  }
  return facts;
}

s_facts * facts_lock_unlock_r (s_facts *facts)
{
  pthread_t thread;
  assert(facts);
  thread = pthread_self();
  if (facts->rwlock_thread != thread &&
      pthread_rwlock_unlock(&facts->rwlock)) {
    err_puts("facts_lock_unlock_r: pthread_rwlock_unlock");
    assert(! "facts_lock_unlock_r: pthread_rwlock_unlock");
    return NULL;
  }
  return facts;
}

s_facts * facts_lock_unlock_w (s_facts *facts)
{
  assert(facts);
  assert(facts->rwlock_count);
  facts->rwlock_count--;
  if (! facts->rwlock_count) {
    facts->rwlock_thread = 0;
    if (pthread_rwlock_unlock(&facts->rwlock)) {
      err_puts("facts_lock_unlock_w: pthread_rwlock_unlock");
      assert(! "facts_lock_unlock_w: pthread_rwlock_unlock");
      return NULL;
    }
  }
  return facts;
}

s_facts * facts_lock_w (s_facts *facts)
{
  pthread_t thread;
  assert(facts);
  thread = pthread_self();
  if (facts->rwlock_thread != thread) {
    if (pthread_rwlock_wrlock(&facts->rwlock)) {
      err_puts("facts_lock_w: pthread_rwlock_wrlock");
      assert(! "facts_lock_w: pthread_rwlock_wrlock");
      return NULL;
    }
    facts->rwlock_thread = thread;
    facts->rwlock_count = 0;
  }
  facts->rwlock_count++;
  return facts;
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

s_facts * facts_new (void)
{
  s_facts *facts;
  facts = alloc(sizeof(s_facts));
  if (! facts)
    return NULL;
  if (! facts_init(facts)) {
    free(facts);
    return NULL;
  }
  return facts;
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
  char i[BUF_SIZE];
  s_buf in;
  sw r;
  sw result = 0;
  buf_init(&in, false, sizeof(i), i);
  fp = fopen(path->ptr.pchar, "rb");
  if (! fp) {
    if (errno == ENOENT)
      return facts_open_file_create(facts, path);
    return -1;
  }
  buf_file_open_r(&in, fp);
  if ((r = facts_open_buf(facts, &in, path)) < 0)
    return r;
  result += r;
  buf_file_close(&in);
  fp = file_open(path->ptr.pchar, "ab");
  if (! fp)
    return -1;
  if (! (facts->log = log_new()))
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
  fp = file_open(path->ptr.pchar, "wb");
  if (! fp)
    return -1;
  if (facts_count(facts)) {
    /* TODO: clear facts
       facts_close(facts);
       facts_remove_all(facts);
    */
    err_puts("facts_open_file: not implemented");
    assert(! "facts_open_file: not implemented");
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
  if (! item) {
    err_write_1("facts_ref_tag: set_add__tag: ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "facts_ref_tag: set_add__tag");
    return NULL;
  }
  item->usage++;
  return &item->data;
}

void facts_remove_all (s_facts *facts)
{
  uw count;
  s_set_cursor__fact cursor;
  s_fact **f;
  uw i;
  uw j;
  s_set_item__fact *item;
  assert(facts);
  count = facts->facts.count;
  if (! count)
    return;
  f = alloc(count * sizeof(s_fact *));
  if (! f) {
    abort();
    return;
  }
  i = 0;
  set_cursor_init__fact(&facts->facts, &cursor);
  while (i < count &&
         (item = set_cursor_next__fact(&cursor))) {
    f[i] = &item->data;
    i++;
  }
  qsort(f, i, sizeof(s_fact *), facts_compare_pfact_id_reverse);
  j = 0;
  while (j < i) {
    facts_remove_fact(facts, f[j]);
    j++;
  }
  free(f);
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

bool facts_remove_fact_tags (s_facts *facts, const s_tag *subject,
                             const s_tag *predicate,
                             const s_tag *object)
{
  s_fact fact;
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  fact.subject = subject;
  fact.predicate = predicate;
  fact.object = object;
  return facts_remove_fact(facts, &fact);
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
    list = list_new(list);
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

sw facts_save_file (s_facts *facts, const char *path)
{
  char b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw r;
  sw result = 0;
  assert(facts);
  assert(path);
  assert(! facts->log);
  buf_init(&buf, false, sizeof(b), b);
  fp = file_open(path, "wb");
  if (! fp)
    return -1;
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

s_facts * facts_transaction_rollback (s_facts *facts,
                                      s_facts_transaction *transaction)
{
  s_fact_action *log;
  s_facts_transaction *t;
  while (t) {
    if (t == transaction)
      goto rollback;
    t = t->next;
  }
  err_puts("facts_transaction_rollback: transaction not found");
  assert(! "facts_transaction_rollback: transaction not found");
  return NULL;
 rollback:
  while (t) {
    log = t->log;
    while (log) {
      if (log->remove) {
        if (! facts_add(facts, &log->fact))
          return NULL;
      }
      else {
        if (! facts_remove(facts, &log->fact))
          return NULL;
      }
      log = log->next;
    }
    if (t == transaction)
      return facts;
    t = t->next;
  }
  err_puts("facts_transaction_rollback: unknown error");
  assert(! "facts_transaction_rollback: unknown error");
  exit(1);
  return NULL;
}

void facts_transaction_start (s_facts *facts,
                              s_facts_transaction *transaction)
{
  assert(facts);
  assert(transaction);
  transaction->next = facts->transaction;
  facts->transaction = transaction;
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
