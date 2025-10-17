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
#include <stdlib.h>
#include "assert.h"
#include <config.h>
#include "facts.h"
#include "facts_cursor.h"
#include "pvar.h"
#include "rwlock.h"
#include "skiplist__fact.h"
#include "skiplist_node__fact.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

void facts_cursor_clean (s_facts_cursor *cursor)
{
  assert(cursor);
  if (cursor->pvar_subject) {
    var_reset(cursor->pvar_subject);
    pvar_clean(&cursor->pvar_subject);
    cursor->pvar_subject = NULL;
  }
  if (cursor->pvar_predicate) {
    var_reset(cursor->pvar_predicate);
    pvar_clean(&cursor->pvar_predicate);
    cursor->pvar_predicate = NULL;
  }
  if (cursor->pvar_object) {
    var_reset(cursor->pvar_object);
    pvar_clean(&cursor->pvar_object);
    cursor->pvar_object = NULL;
  }
#if HAVE_PTHREAD
  facts_cursor_lock_clean(cursor);
#endif
}

s_facts_cursor * facts_cursor_init (s_facts *facts,
                                    s_facts_cursor *cursor,
                                    s_skiplist__fact *index,
                                    s_fact *start,
                                    s_fact *end)
{
  s_skiplist_node__fact *pred;
  s_facts_cursor tmp = {0};
  assert(cursor);
  assert(index);
  pred = skiplist_pred__fact(index, start);
  if (! pred)
    return NULL;
  tmp.index = index;
  tmp.node = SKIPLIST_NODE_NEXT__fact(pred, 0);
  skiplist_node_delete__fact(pred);
  if (start) {
    tmp.start = *start;
    if (tmp.start.subject->type == TAG_PVAR)
      tmp.start.subject = &tmp.start.subject->data.pvar->tag;
    if (tmp.start.predicate->type == TAG_PVAR)
      tmp.start.predicate = &tmp.start.predicate->data.pvar->tag;
    if (tmp.start.object->type == TAG_PVAR)
      tmp.start.object = &tmp.start.object->data.pvar->tag;
  }
  else {
    tmp.start.subject   = TAG_FIRST;
    tmp.start.predicate = TAG_FIRST;
    tmp.start.object    = TAG_FIRST;
    tmp.start.id        = 0;
  }
  if (end) {
    tmp.end = *end;
    if (tmp.end.subject->type == TAG_PVAR)
      tmp.end.subject = &tmp.end.subject->data.pvar->tag;
    if (tmp.end.predicate->type == TAG_PVAR)
      tmp.end.predicate = &tmp.end.predicate->data.pvar->tag;
    if (tmp.end.object->type == TAG_PVAR)
      tmp.end.object = &tmp.end.object->data.pvar->tag;
  }
  else {
    tmp.end.subject   = TAG_LAST;
    tmp.end.predicate = TAG_LAST;
    tmp.end.object    = TAG_LAST;
    tmp.end.id        = (uw) -1;
  }
  tmp.facts = facts;
#if HAVE_PTHREAD
  if (! facts_cursor_lock_init(&tmp)) {
    facts_cursor_clean(&tmp);
    return NULL;
  }
#endif
  *cursor = tmp;
  return cursor;
}

#if HAVE_PTHREAD

s_facts_cursor * facts_cursor_lock (s_facts_cursor *cursor)
{
  assert(cursor);
  if (pthread_mutex_lock(&cursor->mutex)) {
    err_puts("facts_cursor_lock: pthread_mutex_lock");
    assert(! "facts_cursor_lock: pthread_mutex_lock");
    return NULL;
  }
  rwlock_r(&cursor->facts->rwlock);
  return cursor;
}

s_facts_cursor * facts_cursor_lock_clean (s_facts_cursor *cursor)
{
  assert(cursor);
  if (pthread_mutex_destroy(&cursor->mutex)) {
    err_puts("facts_cursor_lock_clean: pthread_mutex_destroy");
    assert(! "facts_cursor_lock_clean: pthread_mutex_destroy");
    return NULL;
  }
  return cursor;
}

s_facts_cursor * facts_cursor_lock_init (s_facts_cursor *cursor)
{
  assert(cursor);
  if (pthread_mutex_init(&cursor->mutex, NULL)) {
    err_puts("facts_cursor_lock_init: pthread_mutex_init");
    assert(! "facts_cursor_lock_init: pthread_mutex_init");
    abort();
    return NULL;
  }
  return cursor;
}

s_facts_cursor * facts_cursor_lock_unlock (s_facts_cursor *cursor)
{
  assert(cursor);
  rwlock_unlock_r(&cursor->facts->rwlock);
  if (pthread_mutex_unlock(&cursor->mutex)) {
    err_puts("facts_cursor_lock_unlock: pthread_mutex_unlock");
    assert(! "facts_cursor_lock_unlock: pthread_mutex_unlock");
    abort();
    return NULL;
  }
  return cursor;
}

#endif /* HAVE_PTHREAD */

s_fact ** facts_cursor_next (s_facts_cursor *cursor,
                             s_fact **dest)
{
  s_fact *fact;
  const s_sym *type;
  assert(cursor);
#if HAVE_PTHREAD
  if (! facts_cursor_lock(cursor))
    return NULL;
#endif
  if (cursor->node) {
  next:
    cursor->node = SKIPLIST_NODE_NEXT__fact(cursor->node, 0);
    if (cursor->node &&
        cursor->index->compare(&cursor->end, cursor->node->fact) < 0)
      cursor->node = NULL;
  }
  if (! cursor->node) {
    if (cursor->pvar_subject) {
      var_reset(cursor->pvar_subject);
      pvar_clean(&cursor->pvar_subject);
      cursor->pvar_subject = NULL;
    }
    if (cursor->pvar_predicate) {
      var_reset(cursor->pvar_predicate);
      pvar_clean(&cursor->pvar_predicate);
      cursor->pvar_predicate = NULL;
    }
    if (cursor->pvar_object) {
      var_reset(cursor->pvar_object);
      pvar_clean(&cursor->pvar_object);
      cursor->pvar_object = NULL;
    }
#if HAVE_PTHREAD
    facts_cursor_lock_unlock(cursor);
#endif
    *dest = NULL;
    return dest;
  }
  fact = cursor->node->fact;
  if (cursor->pvar_subject) {
    var_reset(cursor->pvar_subject);
    if (! tag_type(fact->subject, &type))
      goto ko;
    if (cursor->pvar_subject->type != &g_sym_Tag &&
        cursor->pvar_subject->type != type)
      goto next;
    if (! var_set(cursor->pvar_subject, fact->subject))
      goto ko;
  }
  if (cursor->pvar_predicate) {
    var_reset(cursor->pvar_predicate);
    if (! tag_type(fact->predicate, &type))
      goto ko;
    if (cursor->pvar_predicate->type != &g_sym_Tag &&
        cursor->pvar_predicate->type != type)
      goto next;
    if (! var_set(cursor->pvar_predicate, fact->predicate))
      goto ko;
  }
  if (cursor->pvar_object) {
    var_reset(cursor->pvar_object);
    if (! tag_type(fact->object, &type))
      goto ko;
    if (cursor->pvar_object->type != &g_sym_Tag &&
        cursor->pvar_object->type != type)
      goto next;
    if (! var_set(cursor->pvar_object, fact->object))
      goto ko;
  }
#if HAVE_PTHREAD
  facts_cursor_lock_unlock(cursor);
#endif
  *dest = fact;
  return dest;
 ko:
  if (cursor->pvar_subject)
    var_reset(cursor->pvar_subject);
  if (cursor->pvar_predicate)
    var_reset(cursor->pvar_predicate);
  if (cursor->pvar_object)
    var_reset(cursor->pvar_object);
#if HAVE_PTHREAD
  facts_cursor_lock_unlock(cursor);
#endif
  return NULL;
}
