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
#include <stdlib.h>
#include "assert.h"
#include "config.h"
#include "facts.h"
#include "facts_cursor.h"
#include "skiplist__fact.h"
#include "skiplist_node__fact.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

void facts_cursor_clean (s_facts_cursor *cursor)
{
  assert(cursor);
  if (cursor->var_subject.ptr)
    var_reset(&cursor->var_subject);
  if (cursor->var_predicate.ptr)
    var_reset(&cursor->var_predicate);
  if (cursor->var_object.ptr)
    var_reset(&cursor->var_object);
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
  if (start)
    tmp.start = *start;
  else {
    tmp.start.subject   = TAG_FIRST;
    tmp.start.predicate = TAG_FIRST;
    tmp.start.object    = TAG_FIRST;
  }
  if (end)
    tmp.end = *end;
  else {
    tmp.end.subject   = TAG_LAST;
    tmp.end.predicate = TAG_LAST;
    tmp.end.object    = TAG_LAST;
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
  facts_lock_r(cursor->facts);
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
  facts_lock_unlock_r(cursor->facts);
  if (pthread_mutex_unlock(&cursor->mutex)) {
    err_puts("facts_cursor_lock_unlock: pthread_mutex_unlock");
    assert(! "facts_cursor_lock_unlock: pthread_mutex_unlock");
    abort();
    return NULL;
  }
  return cursor;
}

#endif /* HAVE_PTHREAD */

const s_fact ** facts_cursor_next (s_facts_cursor *cursor,
                                   const s_fact **dest)
{
  const s_fact *fact;
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
    if (cursor->var_subject.ptr)
      var_reset(&cursor->var_subject);
    if (cursor->var_predicate.ptr)
      var_reset(&cursor->var_predicate);
    if (cursor->var_object.ptr)
      var_reset(&cursor->var_object);
#if HAVE_PTHREAD
    facts_cursor_lock_unlock(cursor);
#endif
    *dest = NULL;
    return dest;
  }
  fact = cursor->node->fact;
  if (cursor->var_subject.ptr) {
    var_reset(&cursor->var_subject);
    if (! tag_type(fact->subject, &type))
      goto ko;
    if (cursor->var_subject.type != &g_sym_Tag &&
        cursor->var_subject.type != type)
      goto next;
    if (! var_set(&cursor->var_subject, fact->subject))
      goto ko;
  }
  if (cursor->var_predicate.ptr) {
    var_reset(&cursor->var_predicate);
    if (! tag_type(fact->predicate, &type))
      goto ko;
    if (cursor->var_predicate.type != &g_sym_Tag &&
        cursor->var_predicate.type != type)
      goto next;
    if (! var_set(&cursor->var_predicate, fact->predicate))
      goto ko;
  }
  if (cursor->var_object.ptr) {
    var_reset(&cursor->var_object);
    if (! tag_type(fact->object, &type))
      goto ko;
    if (cursor->var_object.type != &g_sym_Tag &&
        cursor->var_object.type != type)
      goto next;
    if (! var_set(&cursor->var_object, fact->object))
      goto ko;
  }
#if HAVE_PTHREAD
  facts_cursor_lock_unlock(cursor);
#endif
  *dest = fact;
  return dest;
 ko:
  if (cursor->var_subject.ptr)
    var_reset(&cursor->var_subject);
  if (cursor->var_predicate.ptr)
    var_reset(&cursor->var_predicate);
  if (cursor->var_object.ptr)
    var_reset(&cursor->var_object);
#if HAVE_PTHREAD
  facts_cursor_lock_unlock(cursor);
#endif
  return NULL;
}
