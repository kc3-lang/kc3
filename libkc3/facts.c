/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "compare.h"
#include <config.h>
#include "env.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_transaction.h"
#include "facts_with.h"
#include "file.h"
#include "io.h"
#include "list.h"
#include "log.h"
#include "marshall.h"
#include "marshall_read.h"
#include "mutex.h"
#include "rwlock.h"
#include "securelevel.h"
#include "set__fact.h"
#include "set__tag.h"
#include "set_cursor__fact.h"
#include "skiplist__fact.h"
#include "str.h"
#include "sym.h"
#include "tag.h"

static int facts_compare_pfact_id_reverse (const void *a,
                                           const void *b);
static sw facts_open_file_create (s_facts *facts, const s_str *path);
static sw facts_open_log (s_facts *facts, s_buf *buf);

s_fact * facts_add_fact (s_facts *facts, s_fact *fact)
{
  s_fact tmp = {0};
  s_fact *f = NULL;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
  if (securelevel(0) > 1 &&
      facts == env_global()->facts) {
    err_puts("facts_add_fact: cannot add fact to env facts with"
             " securelevel > 1");
    abort();
  }
#if HAVE_PTHREAD
  if (! rwlock_w(&facts->rwlock))
    return NULL;
#endif
  tmp.subject = facts_ref_tag(facts, fact->subject);
  if (! tmp.subject) {
    err_puts("facts_add_fact: facts_ref_tag subject");
    assert(! "facts_add_fact: facts_ref_tag subject");
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  tmp.predicate = facts_ref_tag(facts, fact->predicate);
  if (! tmp.predicate) {
    err_puts("facts_add_fact: facts_ref_tag predicate");
    assert(! "facts_add_fact: facts_ref_tag predicate");
    facts_unref_tag(facts, tmp.subject);
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  tmp.object = facts_ref_tag(facts, fact->object);
  if (! tmp.object) {
    err_puts("facts_add_fact: facts_ref_tag object");
    assert(! "facts_add_fact: facts_ref_tag object");
    facts_unref_tag(facts, tmp.subject);
    facts_unref_tag(facts, tmp.predicate);
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  tmp.id = 0;
  if ((item = set_get__fact(&facts->facts, &tmp))) {
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return &item->data;
  }
  tmp.id = facts->next_id;
  if (facts->next_id == UW_MAX) {
    err_puts("facts_add_fact: facts serial id exhausted");
    assert(! "facts_add_fact: facts serial id exhausted");
    goto ko;
  }
  item = set_add__fact(&facts->facts, &tmp);
  if (! item) {
    err_puts("facts_add_fact: set_add__fact");
    assert(! "facts_add_fact: set_add__fact");
    goto ko;
  }
  f = &item->data;
  if (! skiplist_insert__fact(facts->index, f)) {
    err_puts("facts_add_fact: skiplist_insert__fact index");
    assert(! "facts_add_fact: skiplist_insert__fact index");
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (! skiplist_insert__fact(facts->index_spo, f)) {
    err_puts("facts_add_fact: skiplist_insert__fact spo");
    assert(! "facts_add_fact: skiplist_insert__fact spo");
    skiplist_remove__fact(facts->index, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (! skiplist_insert__fact(facts->index_pos, f)) {
    err_puts("facts_add_fact: skiplist_insert__fact pos");
    assert(! "facts_add_fact: skiplist_insert__fact pos");
    skiplist_remove__fact(facts->index, f);
    skiplist_remove__fact(facts->index_spo, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (! skiplist_insert__fact(facts->index_osp, f)) {
    err_puts("facts_add_fact: skiplist_insert__fact osp");
    assert(! "facts_add_fact: skiplist_insert__fact osp");
    skiplist_remove__fact(facts->index, f);
    skiplist_remove__fact(facts->index_spo, f);
    skiplist_remove__fact(facts->index_pos, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  if (facts->log &&
      ! facts_log_add(facts->log, &tmp)) {
    err_puts("facts_add_fact: facts_log_add");
    assert(! "facts_add_fact: facts_log_add");
    skiplist_remove__fact(facts->index, f);
    skiplist_remove__fact(facts->index_spo, f);
    skiplist_remove__fact(facts->index_pos, f);
    skiplist_remove__fact(facts->index_osp, f);
    set_remove__fact(&facts->facts, f);
    goto ko;
  }
  facts->next_id++;
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  if (false) {
    err_write_1("facts_add_fact: ");
    err_inspect_fact(f);
    err_write_1(": OK\n");
  }
  return f;
 ko:
  facts_unref_tag(facts, tmp.subject);
  facts_unref_tag(facts, tmp.predicate);
  facts_unref_tag(facts, tmp.object);
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return NULL;
}

s_fact * facts_add_tags (s_facts *facts, s_tag *subject,
                         s_tag *predicate, s_tag *object)
{
  s_fact fact;
  if (securelevel(0) > 1 &&
      facts == env_global()->facts) {
    err_puts("facts_add_tags: cannot add tags to env facts with"
             " securelevel > 1");
    abort();
  }
  fact_init(&fact, subject, predicate, object);
  return facts_add_fact(facts, &fact);
}

void facts_clean (s_facts *facts)
{
  if (facts->log)
    facts_close(facts);
  facts_remove_all(facts);
  skiplist_delete__fact(facts->index);
  skiplist_delete__fact(facts->index_osp);
  skiplist_delete__fact(facts->index_pos);
  skiplist_delete__fact(facts->index_spo);
  set_clean__fact(&facts->facts);
  set_clean__tag(&facts->tags);
#if HAVE_PTHREAD
  rwlock_clean(&facts->rwlock);
  mutex_clean(&facts->ref_count_mutex);
#endif
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
  if (! facts) {
    err_puts("facts_delete: invalid argument");
    err_stacktrace();
    assert(! "facts_delete: invalid argument");
    return;
  }
#if HAVE_PTHREAD
  mutex_lock(&facts->ref_count_mutex);
#endif
  if (facts->ref_count <= 0) {
    err_puts("facts_delete: invalid reference count");
    assert(! "facts_delete: invalid reference count");
    abort();
  }
  facts->ref_count--;
  if (false) {
    err_write_1("facts_delete: ");
    if (facts->log) {
      err_inspect_str(&facts->log->path);
      err_write_1(": ");
    }
    err_inspect_uw_decimal(facts->ref_count);
    err_write_1("\n");
  }
  if (facts->ref_count) {
#if HAVE_PTHREAD
    mutex_unlock(&facts->ref_count_mutex);
#endif
    return;
  }
#if HAVE_PTHREAD
  mutex_unlock(&facts->ref_count_mutex);
#endif
  facts_clean(facts);
  free(facts);
}

sw facts_dump (s_facts *facts, s_buf *buf)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag object;
  s_tag predicate;
  sw r;
  sw result = 0;
  s_tag subject;
  assert(facts);
  assert(buf);
  tag_init_pvar(&subject, &g_sym_Tag);
  tag_init_pvar(&predicate, &g_sym_Tag);
  tag_init_pvar(&object, &g_sym_Tag);
  if ((r = buf_write_1(buf,
                       "%{module: KC3.Facts.Dump,\n"
                       "  version: 1}\n")) < 0)
    return r;
  result += r;
#if HAVE_PTHREAD
  rwlock_r(&facts->rwlock);
#endif
  facts_with_0_id(facts, &cursor, subject.data.pvar, predicate.data.pvar,
                  object.data.pvar);
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
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return r;
}

sw facts_dump_file (s_facts *facts, const s_str *path)
{
  char b[BUF_SIZE];
  s_buf buf;
  s_env *env;
  FILE *fp;
  sw r = -1;
  assert(facts);
  assert(path);
  env = env_global();
  assert(env);
  if (env->trace) {
    err_write_1("facts_dump_file: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  buf_init(&buf, false, sizeof(b), b);
  fp = file_open(path, "wb");
  if (fp) {
    buf_file_open_w(&buf, fp);
    r = facts_dump(facts, &buf);
    buf_file_close(&buf);
    fclose(fp);
  }
  if (env->trace) {
    err_write_1("facts_dump_file: ");
    err_inspect_str(path);
    err_write_1(r > 0 ? ": OK\n" : ": ERROR\n");
  }
  return r;
}

s_fact ** facts_find_fact (s_facts *facts, const s_fact *fact,
                           s_fact **dest)
{
  s_fact f;
  s_set_item__fact *item;
  assert(facts);
  assert(fact);
#if HAVE_PTHREAD
  if (! rwlock_r(&facts->rwlock))
    return NULL;
#endif
  if (! facts_find_tag(facts, fact->subject, &f.subject)) {
    err_puts("facts_find_fact: facts_find_tag subject");
    assert(! "facts_find_fact: facts_find_tag subject");
    goto clean;
  }
  if (! facts_find_tag(facts, fact->predicate, &f.predicate)) {
    err_puts("facts_find_fact: facts_find_tag predicate");
    assert(! "facts_find_fact: facts_find_tag predicate");
    goto clean;
  }
  if (! facts_find_tag(facts, fact->object, &f.object)) {
    err_puts("facts_find_fact: facts_find_tag object");
    assert(! "facts_find_fact: facts_find_tag object");
    goto clean;
  }
  f.id = 0;
  *dest = NULL;
  if (f.subject && f.predicate && f.object &&
      (item = set_get__fact(&facts->facts, &f)))
    *dest = &item->data;
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return dest;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return NULL;
}

s_fact ** facts_find_fact_by_tags (s_facts *facts, s_tag *subject,
                                   s_tag *predicate, s_tag *object,
                                   s_fact **dest)
{
  s_fact f = {subject, predicate, object, 0};
  return facts_find_fact(facts, &f, dest);
}

s_tag ** facts_find_tag (s_facts *facts, const s_tag *tag, s_tag **dest)
{
  s_set_item__tag *item;
  assert(facts);
  assert(tag);
#if HAVE_PTHREAD
  if (! rwlock_r(&facts->rwlock))
    return NULL;
#endif
  *dest = NULL;
  if ((item = set_get__tag(&facts->tags, tag)))
    *dest = &item->data;
#if HAVE_PTHREAD
  rwlock_unlock_r(&facts->rwlock);
#endif
  return dest;
}

s_facts * facts_init (s_facts *facts)
{
  const u8 max_height = 20;
  const double spacing = 2.7;
  s_facts tmp = {0};
  assert(facts);
  set_init__tag(&tmp.tags, 1024);
  set_init__fact(&tmp.facts, 1024);
  // id index
  tmp.index = skiplist_new__fact(max_height, spacing);
  assert(tmp.index);
  tmp.index->compare = compare_fact_id;
  // spo index
  tmp.index_spo = skiplist_new__fact(max_height, spacing);
  assert(tmp.index_spo);
  tmp.index_spo->compare = compare_fact;
  // pos index
  tmp.index_pos = skiplist_new__fact(max_height, spacing);
  assert(tmp.index_pos);
  tmp.index_pos->compare = compare_fact_pos;
  // osp index
  tmp.index_osp = skiplist_new__fact(max_height, spacing);
  assert(tmp.index_osp);
  tmp.index_osp->compare = compare_fact_osp;
  
#if HAVE_PTHREAD
  rwlock_init(&tmp.rwlock);
  mutex_init(&tmp.ref_count_mutex);
#endif
  tmp.next_id = 1;
  tmp.ref_count = 1;
  *facts = tmp;
  return facts;
}

sw facts_load (s_facts *facts, s_buf *buf, const s_str *path)
{
  s_fact   eval_fact;
  s_fact_w eval_fact_w;
  s_fact_w fact_w;
  sw r;
  bool replace;
  sw result = 0;
  assert(facts);
  assert(buf);
  if (env_global()->trace) {
    err_write_1("facts_load: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  if ((r = buf_read_1(buf,
                      "%{module: KC3.Facts.Dump,\n"
                      "  version: 1}\n")) <= 0) {
    err_write_1("facts_load: invalid or missing header: ");
    err_puts(path->ptr.pchar);
    assert(! "facts_load: invalid or missing header");
    return -1;
  }
  result += r;
#if HAVE_PTHREAD
  rwlock_w(&facts->rwlock);
#endif
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
    if ((r = buf_parse_fact_w(buf, &fact_w)) <= 0) {
      err_write_1("facts_load: invalid fact line ");
      err_inspect_sw_decimal(buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      err_write_1("\n");
      assert(! "facts_load: invalid fact");
      goto ko;
    }
    result += r;
    if ((r = buf_read_1(buf, "\n")) <= 0) {
      fact_w_clean(&fact_w);
      err_write_1("facts_load: missing newline line ");
      err_inspect_sw_decimal(buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      assert(! "facts_load: missing newline");
      goto ko;
    }
    result += r;
    if (! fact_w_eval(&fact_w, &eval_fact_w)) {
      fact_w_clean(&fact_w);
      err_write_1("facts_load: failed to eval fact line ");
      err_inspect_sw_decimal(buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      assert(! "facts_load: invalid fact");
      goto ko;
    }
    if (! fact_init_fact_w(&eval_fact, &eval_fact_w)) {
      fact_w_clean(&eval_fact_w);
      fact_w_clean(&fact_w);
      err_write_1("facts_load: failed to eval fact line ");
      err_inspect_sw_decimal(buf->line);
      err_write_1(": ");
      err_puts(path->ptr.pchar);
      err_inspect_buf(buf);
      assert(! "facts_load: invalid fact");
      goto ko;
    }
    if (replace) {
      if (! facts_replace_fact(facts, &eval_fact)) {
	fact_w_clean(&eval_fact_w);
	fact_w_clean(&fact_w);
	err_write_1("facts_load: failed to replace fact line ");
	err_inspect_sw_decimal(buf->line);
	err_write_1(": ");
	err_puts(path->ptr.pchar);
	assert(! "facts_load: failed to replace fact");
	goto ko;
      }
    }
    else {
      if (! facts_add_fact(facts, &eval_fact)) {
	fact_w_clean(&eval_fact_w);
	fact_w_clean(&fact_w);
	err_write_1("facts_load: failed to add fact line ");
	err_inspect_sw_decimal(buf->line);
	err_write_1(": ");
	err_puts(path->ptr.pchar);
	assert(! "facts_load: failed to add fact");
	goto ko;
      }
    }
    fact_w_clean(&fact_w);
    fact_w_clean(&eval_fact_w);
  }
  if (env_global()->trace) {
    err_write_1("facts_load: ");
    err_inspect_str(path);
    err_write_1(": OK\n");
  }
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return result;
 ko:
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return -1;
}

sw facts_load_binary (s_facts *facts, const s_str *path)
{
  u8 action;
  s_env *env;
  s_fact fact;
  uw i;
  s_marshall_read mr = {0};
  sw result = -1;
  assert(facts);
  assert(path);
  env = env_global();
  assert(env);
  if (env->trace) {
    err_write_1("facts_load_binary: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  if (! marshall_read_init_file(&mr, path)) {
    err_puts("facts_load_binary: marshall_read_init_file");
    assert(! "facts_load_binary: marshall_read_init_file");
    goto ko;
  }
#if HAVE_PTHREAD
  rwlock_w(&facts->rwlock);
#endif
  i = 0;
  while (1) {
    i++;
    if (! marshall_read_u8(&mr, false, &action))
      break;
    if (! marshall_read_fact(&mr, false, &fact)) {
      err_write_1("facts_load_binary: invalid fact #");
      err_inspect_uw_decimal(i);
      err_write_1(": ");
      err_inspect_str(path);
      err_write_1("\n");
      assert(! "facts_load_binary: invalid fact");
      goto ko_unlock;
    }
    switch (action) {
    case FACT_ACTION_REPLACE:
      if (! facts_replace_fact(facts, &fact)) {
	fact_clean_all(&fact);
	err_write_1("facts_load_binary: failed to replace fact #");
	err_inspect_uw_decimal(i);
	err_write_1(": ");
	err_inspect_str(path);
        err_write_1("\n");
	assert(! "facts_load_binary: failed to replace fact");
	goto ko_unlock;
      }
      break;
    case FACT_ACTION_ADD:
      if (! facts_add_fact(facts, &fact)) {
	fact_clean_all(&fact);
	err_write_1("facts_load_binary: failed to add fact #");
	err_inspect_sw_decimal(i);
	err_write_1(": ");
	err_inspect_str(path);
        err_write_1("\n");
	assert(! "facts_load_binary: failed to add fact");
	goto ko_unlock;
      }
      break;
    default:
      err_puts("facts_load_binary: invalid fact action");
      assert(! "facts_load_binary: invalid fact action");
      goto ko_unlock;
    }
  }
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  result = marshall_read_size(&mr);
  marshall_read_clean(&mr);
  if (env->trace) {
    err_write_1("facts_load_binary: ");
    err_inspect_str(path);
    err_write_1(": OK\n");
  }
  return result;
 ko_unlock:
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
 ko:
  marshall_read_clean(&mr);
  if (env->trace) {
    err_write_1("facts_load_binary: ");
    err_inspect_str(path);
    err_write_1(": ERROR\n");
  }
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
  fp = file_open(path, "rb");
  if (! fp)
    return -1;
  buf_file_open_r(&buf, fp);
  result = facts_load(facts, &buf, path);
  buf_file_close(&buf);
  fclose(fp);
  return result;
}

// TODO: if (binary) {
sw facts_log_add (s_log *log, const s_fact *fact)
{
  sw r;
  sw result = 0;
  assert(log);
  assert(fact);
  if ((r = buf_write_1(&log->buf, "add ")) < 0)
    goto ko;
  result += r;
  if ((r = buf_inspect_fact(&log->buf, fact)) < 0)
    goto ko;
  result += r;
  if ((r = buf_write_1(&log->buf, "\n")) < 0)
    goto ko;
  result += r;
  if (log->binary_path.size) {
    if ((r = buf_write_1(&log->binary_buf, "add ")) < 0)
      goto ko;
    result += r;
    if ((r = buf_inspect_fact(&log->binary_buf, fact)) < 0)
      goto ko;
    result += r;
    if ((r = buf_write_1(&log->binary_buf, "\n")) < 0)
      goto ko;
    result += r;
  }
  return result;
 ko:
  err_puts("facts_log_add: error");
  assert(! "facts_log_add: error");
  return r;
}

// TODO: if (binary) {
sw facts_log_remove (s_log *log, const s_fact *fact)
{
  sw r;
  sw result = 0;
  assert(log);
  assert(fact);
  if ((r = buf_write_1(&log->buf, "remove ")) < 0)
    goto ko;
  result += r;
  if ((r = buf_inspect_fact(&log->buf, fact)) < 0)
    goto ko;
  result += r;
  if ((r = buf_write_1(&log->buf, "\n")) < 0)
    goto ko;
  result += r;
  if (log->binary_path.size) {
    if ((r = buf_write_1(&log->binary_buf, "remove ")) < 0)
      goto ko;
    result += r;
    if ((r = buf_inspect_fact(&log->binary_buf, fact)) < 0)
      goto ko;
    result += r;
    if ((r = buf_write_1(&log->binary_buf, "\n")) < 0)
      goto ko;
    result += r;
  }
  return result;
 ko:
  err_puts("facts_log_remove: error");
  assert(! "facts_log_remove: error");
  return r;
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

s_facts * facts_new_ref (s_facts *src)
{
  assert(src);
#if HAVE_PTHREAD
  mutex_lock(&src->ref_count_mutex);
#endif
  if (src->ref_count <= 0) {
    err_puts("facts_new_ref: invalid reference count");
    assert(! "facts_new_ref: invalid reference count");
    abort();
  }
  src->ref_count++;
  if (false) {
    err_write_1("facts_new_ref: ");
    if (src->log) {
      err_inspect_str(&src->log->path);
      err_write_1(": ");
    }
    err_inspect_uw_decimal(src->ref_count);
    err_write_1("\n");
  }
#if HAVE_PTHREAD
  mutex_unlock(&src->ref_count_mutex);
#endif
  return src;
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
  if (facts_dump_file(facts, path) < 0)
    return -1;
  fp = file_open(path, "ab");
  if (! fp)
    return -1;
  assert(! facts->log);
  if (! (facts->log = log_new()))
    return -1;
  log_open(facts->log, fp, path);
  return result;
}

sw facts_open_file_binary (s_facts *facts, const s_str *path)
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
      return facts_open_file_binary_create(facts, path);
    return -1;
  }
  buf_file_open_r(&in, fp);
  if ((r = facts_open_buf(facts, &in, path)) < 0)
    return r;
  result += r;
  buf_file_close(&in);
  fp = file_open(path, "ab");
  if (! fp)
    return -1;
  if (! facts->log &&
      ! (facts->log = log_new())) {
    err_puts("facts_open_file_binary: log_new");
    assert(! "facts_open_file_binary: log_new");
    return -1;
  }
  if (! log_open_binary(facts->log, fp, path)) {
    err_puts("facts_open_file_binary: log_open_binary");
    assert(! "facts_open_file_binary: log_open_binary");
    return -1;
  }
  return result;
}

sw facts_open_file_binary_create (s_facts *facts, const s_str *path)
{
  FILE *fp;
  fp = file_open(path, "wb");
  if (! fp)
    return -1;
  if (! facts->log &&
      ! (facts->log = log_new())) {
    fclose(fp);
    return -1;
  }
  if (! log_open_binary(facts->log, fp, path)) {
    fclose(fp);
    return -1;
  }
  return buf_write_1(&facts->log->binary_buf,
                     "%{module: KC3.Facts.Dump,\n"
                     "  version: 1}\n");
}

sw facts_open_file_create (s_facts *facts, const s_str *path)
{
  FILE *fp;
  s_buf *out;
  sw r;
  sw result = 0;
  fp = file_open(path, "wb");
  if (! fp)
    return -1;
  out = buf_new_alloc(BUF_SIZE);
  buf_file_open_w(out, fp);
  if ((r = facts_dump(facts, out)) < 0)
    return r;
  result += r;
  buf_flush(out);
  if (! (facts->log = log_new()))
    return -1;
  if (! log_open(facts->log, fp, path)) {
    log_delete(facts->log);
    facts->log = NULL;
    return -1;
  }
  return result;
}

// TODO: if (binary) {
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
      if ((r = buf_parse_fact_w(buf, &fact_w)) <= 0)
        break;
      result += r;
      if (! fact_init_fact_w(&fact, &fact_w))
        return -1;
      if (! facts_add_fact(facts, &fact))
        return -1;
      goto ok;
    }
    if ((r = buf_read_1(buf, "remove ")) <= 0)
      break;
    result += r;
    if ((r = buf_parse_fact_w(buf, &fact_w)) <= 0)
      break;
    result += r;
    if (! fact_init_fact_w(&fact, &fact_w))
      return -1;
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

s_tag * facts_ref_tag (s_facts *facts, s_tag *tag)
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
  if (! env_cleaning(false) &&
      securelevel(0) > 1 &&
      facts == env_global()->facts) {
    err_puts("facts_remove_all: cannot remove all facts from env facts"
             " with securelevel > 1 unless cleaning global env");
    abort();
  }
  count = facts->facts.count;
  if (! count)
    return facts;
  f = alloc(count * sizeof(s_fact *));
  if (! f)
    return NULL;
  // TODO: FIXME
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
  s_fact *found;
  assert(facts);
  assert(fact);
  if (! env_cleaning(false) &&
      securelevel(0) > 1 &&
      facts == env_global()->facts) {
    err_puts("facts_remove_fact: cannot remove fact from env facts with"
             " securelevel > 1 unless cleaning global env");
    abort();
  }
#if HAVE_PTHREAD
  rwlock_w(&facts->rwlock);
#endif
  if (! facts_find_fact(facts, fact, &found)) {
    err_puts("facts_remove_fact: facts_find_fact");
    assert(! "facts_remove_fact: facts_find_fact");
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  if (found) {
    if (false) {
      err_write_1("facts_remove_fact: ");
      err_inspect_fact(found);
      err_write_1("\n");
    }
    if (facts->log)
      facts_log_remove(facts->log, found);
    skiplist_remove__fact(facts->index, found);
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
  else
    *dest = false;
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return dest;
}

bool * facts_remove_fact_tags (s_facts *facts, s_tag *subject,
                               s_tag *predicate,
                               s_tag *object,
                               bool *dest)
{
  s_fact fact;
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  if (! env_cleaning(false) &&
      securelevel(0) > 1 &&
      facts == env_global()->facts) {
    err_puts("facts_remove_fact_tags: cannot remove tags from env facts"
             " with securelevel > 1");
    abort();
  }
  fact.subject = subject;
  fact.predicate = predicate;
  fact.object = object;
  return facts_remove_fact(facts, &fact, dest);
}

s_fact * facts_replace_fact (s_facts *facts, s_fact *fact)
{
  assert(facts);
  assert(fact);
  return facts_replace_tags(facts, fact->subject, fact->predicate,
                            fact->object);
}

s_fact * facts_replace_tags (s_facts *facts, s_tag *subject,
                             s_tag *predicate,
                             s_tag *object)
{
  bool b;
  s_facts_cursor cursor;
  s_fact *fact;
  s_list *list = NULL;
  s_facts_transaction transaction;
  s_tag pvar;
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  tag_init_pvar(&pvar, &g_sym_Tag);
#if HAVE_PTHREAD
  if (! rwlock_w(&facts->rwlock)) {
    tag_clean(&pvar);
    return NULL;
  }
#endif
  if (! facts_with_tags(facts, &cursor, subject,
                        predicate, &pvar)) {
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    err_puts("facts_replace_tags: facts_with_tags");
    assert(! "facts_replace_tags: facts_with_tags");
    goto clean;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    err_puts("facts_replace_tags: facts_cursor_next 1");
    assert(! "facts_replace_tags: facts_cursor_next 1");
    goto clean;
  }
  while (fact) {
    list = list_new(list);
    list->tag.type = TAG_FACT;
    list->tag.data.fact = *fact;
    if (! facts_cursor_next(&cursor, &fact)) {
#if HAVE_PTHREAD
      rwlock_unlock_w(&facts->rwlock);
#endif
      err_puts("facts_replace_tags: facts_cursor_next 2");
      assert(! "facts_replace_tags: facts_cursor_next 2");
      list_delete_all(list);
      goto clean;
    }
  }
  facts_transaction_start(facts, &transaction);  
  while (list) {
    if (! facts_remove_fact(facts, &list->tag.data.fact, &b)) {
      err_puts("facts_replace_tags: facts_remove_fact");
      assert(! "facts_replace_tags: facts_remove_fact");
      goto rollback;
    }
    if (! b) {
      err_puts("facts_replace_tags: facts_remove_fact:"
               " fact not found");
      assert(!("facts_replace_tags: facts_remove_fact:"
               " fact not found"));
      goto rollback;
    }
    list = list_delete(list);
  }
  tag_clean(&pvar);
  fact = facts_add_tags(facts, subject, predicate, object);
  facts_transaction_end(facts, &transaction);
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return fact;
 rollback:
  facts_transaction_rollback(facts, &transaction);
  list_delete_all(list);
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
 clean:
  tag_clean(&pvar);
  return NULL;
}

sw facts_save_file (s_facts *facts, const s_str *path)
{
  char b[BUF_SIZE];
  s_buf buf;
  FILE *fp;
  sw r;
  sw result = 0;
  assert(facts);
  assert(path);
  if (env_global()->trace) {
    err_write_1("facts_save_file: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  buf_init(&buf, false, sizeof(b), b);
  if (! (fp = file_open(path, "wb"))) {
    r = -1;
    goto ko;
  }
  buf_file_open_w(&buf, fp);
  if ((r = facts_dump(facts, &buf)) < 0)
    goto clean;
  result += r;
  buf_flush(&buf);
  free(buf.user_ptr);
  buf.user_ptr = NULL;
  if (! facts->log &&
      ! (facts->log = log_new()))
    goto clean;
  if (! log_open(facts->log, fp, path))
    goto clean;
  if (env_global()->trace) {
    err_write_1("facts_save_file: ");
    err_inspect_str(path);
    err_puts(": OK");
  }
  return result;
 clean:
  fclose(fp);
 ko:
  if (env_global()->trace) {
    err_write_1("facts_save_file: ");
    err_inspect_str(path);
    err_puts(": ERROR");
  }
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
