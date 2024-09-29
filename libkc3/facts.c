/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "facts_transaction.h"
#include "facts_with.h"
#include "file.h"
#include "io.h"
#include "list.h"
#include "log.h"
#include "set__fact.h"
#include "set__tag.h"
#include "set_cursor__fact.h"
#include "skiplist__fact.h"
#include "sym.h"
#include "tag.h"

static int facts_compare_pfact_id_reverse (const void *a,
                                           const void *b);
static sw facts_open_file_create (s_facts *facts, const s_str *path);
static sw facts_open_log (s_facts *facts, s_buf *buf);

const s_fact * facts_add_fact (s_facts *facts, const s_fact *fact)
{
  s_fact tmp = {0};
  s_fact *f = NULL;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
  if (! facts_lock_w(facts))
    return NULL;
  tmp.subject = facts_ref_tag(facts, fact->subject);
  if (! tmp.subject) {
    facts_lock_unlock_w(facts);
    return NULL;
  }
  tmp.predicate = facts_ref_tag(facts, fact->predicate);
  if (! tmp.predicate) {
    facts_unref_tag(facts, tmp.subject);
    facts_lock_unlock_w(facts);
    return NULL;
  }
  tmp.object = facts_ref_tag(facts, fact->object);
  if (! tmp.object) {
    facts_unref_tag(facts, tmp.subject);
    facts_unref_tag(facts, tmp.predicate);
    facts_lock_unlock_w(facts);
    return NULL;
  }
  tmp.id = 0;
  if ((item = set_get__fact(&facts->facts, &tmp))) {
    facts_lock_unlock_w(facts);
    return &item->data;
  }
  tmp.id = facts->next_id;
  if (facts->next_id == UW_MAX) {
    err_puts("facts_add_fact: facts serial id exhausted");
    assert(! "facts_add_fact: facts serial id exhausted");
    goto ko;
  }
  item = set_add__fact(&facts->facts, &tmp);
  if (! item)
    goto ko;
  f = &item->data;
  if (! skiplist_insert__fact(facts->index_spo, f)) {
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (! skiplist_insert__fact(facts->index_pos, f)) {
    skiplist_remove__fact(facts->index_spo, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (! skiplist_insert__fact(facts->index_osp, f)) {
    skiplist_remove__fact(facts->index_spo, f);
    skiplist_remove__fact(facts->index_pos, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (facts->log &&
      ! facts_log_add(facts->log, &tmp)) {
    skiplist_remove__fact(facts->index_spo, f);
    skiplist_remove__fact(facts->index_pos, f);
    skiplist_remove__fact(facts->index_osp, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  facts->next_id++;
  facts_lock_unlock_w(facts);
  return f;
 ko:
  facts_unref_tag(facts, tmp.subject);
  facts_unref_tag(facts, tmp.predicate);
  facts_unref_tag(facts, tmp.object);
  facts_lock_unlock_w(facts);
  return NULL;
}

const s_fact * facts_add_tags (s_facts *facts, const s_tag *subject,
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

s_facts ** facts_database (s_facts **facts)
{
  *facts = facts_new();
  return facts;
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
  const s_fact *fact;
  s_tag predicate;
  s_tag object;
  sw r;
  sw result = 0;
  s_tag subject;
  assert(facts);
  assert(buf);
  tag_init_var(&subject, &g_sym_Tag);
  tag_init_var(&predicate, &g_sym_Tag);
  tag_init_var(&object, &g_sym_Tag);
  if ((r = buf_write_1(buf,
                       "%{module: KC3.Facts.Dump,\n"
                       "  version: 1}\n")) < 0)
    return r;
  result += r;
  facts_lock_r(facts);
  facts_with_0(facts, &cursor, &subject.data.var, &predicate.data.var,
               &object.data.var);
  if (! facts_cursor_next(&cursor, &fact))
    goto clean;
  while (fact) {
    if ((r = buf_write_1(buf, "add ")) < 0)
      goto clean;
    result += r;
    if ((r = buf_inspect_fact(buf, fact)) < 0)
      goto clean;
    result += r;
    if ((r = buf_write_1(buf, "\n")) < 0)
      goto clean;
    result += r;
    if (! facts_cursor_next(&cursor, &fact))
      goto clean;
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

const s_fact ** facts_find_fact (s_facts *facts, const s_fact *fact,
                                 const s_fact **dest)
{
  s_fact f;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
  if (! facts_lock_r(facts))
    return NULL;
  if (! facts_find_tag(facts, fact->subject, &f.subject))
    return NULL;
  if (! facts_find_tag(facts, fact->predicate, &f.predicate))
    return NULL;
  if (! facts_find_tag(facts, fact->object, &f.object))
    return NULL;
  *dest = NULL;
  if (f.subject && f.predicate && f.object &&
      (item = set_get__fact((const s_set__fact *) &facts->facts, &f)))
    *dest = &item->data;
  facts_lock_unlock_r(facts);
  return dest;
}

const s_fact ** facts_find_fact_by_tags (s_facts *facts,
                                         const s_tag *subject,
                                         const s_tag *predicate,
                                         const s_tag *object,
                                         const s_fact **dest)
{
  s_fact f = {subject, predicate, object, 0};
  return facts_find_fact(facts, &f, dest);
}

const s_tag ** facts_find_tag (s_facts *facts, const s_tag *tag,
                               const s_tag **dest)
{
  s_set_item__tag *item;
  assert(facts);
  assert(tag);
  if (! facts_lock_r(facts))
    return NULL;
  *dest = NULL;
  if ((item = set_get__tag(&facts->tags, tag)))
    *dest = &item->data;
  facts_lock_unlock_r(facts);
  return dest;
}

s_facts * facts_init (s_facts *facts)
{
  const u8 max_height = 10;
  const double spacing = 2.7;
  s_facts tmp = {0};
  assert(facts);
  set_init__tag(&tmp.tags, 1024);
  set_init__fact(&tmp.facts, 1024);
  tmp.index_spo = skiplist_new__fact(max_height, spacing);
  assert(tmp.index_spo);
  tmp.index_spo->compare = compare_fact;
  tmp.index_pos = skiplist_new__fact(max_height, spacing);
  assert(tmp.index_pos);
  tmp.index_pos->compare = compare_fact_pos;
  tmp.index_osp = skiplist_new__fact(max_height, spacing);
  assert(tmp.index_osp);
  tmp.index_osp->compare = compare_fact_osp;
  facts_lock_init(facts);
  tmp.next_id = 1;
  *facts = tmp;
  return facts;
}

sw facts_load (s_facts *facts, s_buf *buf, const s_str *path)
{
  s_fact_w fact;
  s_fact_w fact_eval;
  s_fact   fact_eval_r;
  sw r;
  bool replace;
  sw result = 0;
  assert(facts);
  assert(buf);
  if ((r = buf_read_1(buf,
                      "%{module: KC3.Facts.Dump,\n"
                      "  version: 1}\n")) <= 0) {
    err_write_1("facts_load: invalid or missing header: ");
    err_puts(path->ptr.pchar);
    assert(! "facts_load: invalid or missing header");
    return -1;
  }
  result += r;
  facts_lock_w(facts);
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
    if ((r = buf_parse_fact(buf, &fact)) <= 0) {
      err_write_1("facts_load: invalid fact line ");
      err_inspect_sw_decimal(&buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "facts_load: invalid fact");
      goto ko;
    }
    result += r;
    if ((r = buf_read_1(buf, "\n")) <= 0) {
      fact_w_clean(&fact);
      err_write_1("facts_load: missing newline line ");
      err_inspect_sw_decimal(&buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      assert(! "facts_load: missing newline");
      goto ko;
    }
    result += r;
    if (! fact_w_eval(&fact, &fact_eval)) {
      fact_w_clean(&fact);
      err_write_1("facts_load: failed to eval fact line ");
      err_inspect_sw_decimal(&buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      assert(! "facts_load: invalid fact");
      goto ko;
    }
    fact_r(&fact_eval, &fact_eval_r);
    if (replace) {
      if (! facts_replace_fact(facts, &fact_eval_r)) {
	fact_w_clean(&fact_eval);
	fact_w_clean(&fact);
	err_write_1("facts_load: failed to replace fact line ");
	err_inspect_sw_decimal(&buf->line);
	err_write_1(": ");
	err_puts(path->ptr.pchar);
	assert(! "facts_load: failed to replace fact");
	goto ko;
      }
    }
    else {
      if (! facts_add_fact(facts, &fact_eval_r)) {
	fact_w_clean(&fact_eval);
	fact_w_clean(&fact);
	err_write_1("facts_load: failed to add fact line ");
	err_inspect_sw_decimal(&buf->line);
	err_write_1(": ");
	err_puts(path->ptr.pchar);
	assert(! "facts_load: failed to add fact");
	goto ko;
      }
    }
    fact_w_clean(&fact);
    fact_w_clean(&fact_eval);
  }
  facts_lock_unlock_w(facts);
  return result;
 ko:
  facts_lock_unlock_w(facts);
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
    abort();
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
    abort();
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
  bool b;
  s_fact_w fact_w;
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
      if ((r = buf_parse_fact(buf, &fact_w)) <= 0)
        break;
      result += r;
      fact_r(&fact_w, &fact);
      if (! facts_add_fact(facts, &fact))
        return -1;
      goto ok;
    }
    if ((r = buf_read_1(buf, "remove ")) <= 0)
      break;
    result += r;
    if ((r = buf_parse_fact(buf, &fact_w)) <= 0)
      break;
    result += r;
    fact_r(&fact_w, &fact);
    if (! facts_remove_fact(facts, &fact, &b))
      return -1;
  ok:
    fact_w_clean(&fact_w);
    if ((r = buf_read_1(buf, "\n")) <= 0)
      break;
    result += r;
  }
  return result;
}

const s_tag * facts_ref_tag (s_facts *facts, const s_tag *tag)
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

s_facts * facts_remove_all (s_facts *facts)
{
  bool b;
  uw count;
  s_set_cursor__fact cursor;
  s_fact **f;
  uw i;
  uw j;
  s_set_item__fact *item;
  assert(facts);
  count = facts->facts.count;
  if (! count)
    return facts;
  f = alloc(count * sizeof(s_fact *));
  if (! f)
    return NULL;
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
    if (! facts_remove_fact(facts, f[j], &b) ||
        ! b) {
      free(f);
      return NULL;
    }
    j++;
  }
  free(f);
  return facts;
}

bool * facts_remove_fact (s_facts *facts, const s_fact *fact,
                          bool *dest)
{
  s_fact f;
  const s_fact *found;
  assert(facts);
  assert(fact);
  facts_lock_w(facts);
  if (! facts_find_fact(facts, fact, &found))
    return NULL;
  *dest = false;
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
    *dest = true;
  }
  facts_lock_unlock_w(facts);
  return dest;
}

bool * facts_remove_fact_tags (s_facts *facts, const s_tag *subject,
                               const s_tag *predicate,
                               const s_tag *object,
                               bool *dest)
{
  s_fact fact;
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  fact.subject = subject;
  fact.predicate = predicate;
  fact.object = object;
  return facts_remove_fact(facts, &fact, dest);
}

const s_fact * facts_replace_fact (s_facts *facts, const s_fact *fact)
{
  assert(facts);
  assert(fact);
  return facts_replace_tags(facts, fact->subject, fact->predicate,
                            fact->object);
}

const s_fact * facts_replace_tags (s_facts *facts, const s_tag *subject,
                                   const s_tag *predicate,
                                   const s_tag *object)
{
  bool b;
  s_facts_cursor cursor;
  const s_fact *fact;
  s_list *list = NULL;
  s_facts_transaction transaction;
  s_tag var;
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  tag_init_var(&var, &g_sym_Tag);
  if (! facts_lock_w(facts))
    return NULL;
  if (! facts_with_tags(facts, &cursor, (s_tag *) subject,
                        (s_tag *) predicate, &var)) {
    facts_lock_unlock_w(facts);
    return NULL;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    facts_lock_unlock_w(facts);
    return NULL;
  }
  while (fact) {
    list = list_new(list);
    list->tag.data.fact = *fact;
    if (! facts_cursor_next(&cursor, &fact)) {
      facts_lock_unlock_w(facts);
      list_delete_all(list);
      return NULL;
    }
  }
  facts_transaction_start(facts, &transaction);  
  while (list) {
    if (! facts_remove_fact(facts, &list->tag.data.fact, &b) ||
        ! b) {
      list_delete_all(list);
      facts_transaction_rollback(facts, &transaction);
      facts_lock_unlock_w(facts);
      return NULL;
    }
    list = list_delete(list);
  }
  facts_cursor_clean(&cursor);
  fact = facts_add_tags(facts, subject, predicate, object);
  facts_transaction_end(facts, &transaction);
  facts_lock_unlock_w(facts);
  return fact;
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
