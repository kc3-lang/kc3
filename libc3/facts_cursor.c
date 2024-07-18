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
#include "assert.h"
#include "facts.h"
#include "facts_cursor.h"
#include "skiplist__fact.h"
#include "skiplist_node__fact.h"
#include "tag.h"

void facts_cursor_clean (s_facts_cursor *cursor)
{
  assert(cursor);
  facts_cursor_lock_clean(cursor);
}

s_facts_cursor * facts_cursor_init (s_facts *facts,
                                    s_facts_cursor *cursor,
                                    s_skiplist__fact *index,
                                    s_fact *start,
                                    s_fact *end)
{
  s_skiplist_node__fact *pred;
  assert(cursor);
  assert(index);
  pred = skiplist_pred__fact(index, start);
  assert(pred);
  cursor->index = index;
  cursor->node = SKIPLIST_NODE_NEXT__fact(pred, 0);
  skiplist_node_delete__fact(pred);
  if (start)
    cursor->start = *start;
  else {
    cursor->start.subject   = TAG_FIRST;
    cursor->start.predicate = TAG_FIRST;
    cursor->start.object    = TAG_FIRST;
  }
  if (end)
    cursor->end = *end;
  else {
    cursor->end.subject   = TAG_LAST;
    cursor->end.predicate = TAG_LAST;
    cursor->end.object    = TAG_LAST;
  }
  cursor->var_subject   = NULL;
  cursor->var_predicate = NULL;
  cursor->var_object    = NULL;
  cursor->facts = facts;
  facts_cursor_lock_init(cursor);
  return cursor;
}

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
    return NULL;
  }
  return cursor;
}

s_fact * facts_cursor_next (s_facts_cursor *cursor)
{
  assert(cursor);
  facts_cursor_lock(cursor);
  if (cursor->node) {
    cursor->node = SKIPLIST_NODE_NEXT__fact(cursor->node, 0);
    if (cursor->node &&
        cursor->index->compare(&cursor->end, cursor->node->fact) < 0)
      cursor->node = NULL;
  }
  if (cursor->node) {
    s_fact *fact = cursor->node->fact;
    if (cursor->var_subject)
      *cursor->var_subject = *fact->subject;
    if (cursor->var_predicate)
      *cursor->var_predicate = *fact->predicate;
    if (cursor->var_object)
      *cursor->var_object = *fact->object;
    facts_cursor_lock_unlock(cursor);
    return fact;
  }
  if (cursor->var_subject)
    tag_init_var(cursor->var_subject);
  if (cursor->var_predicate)
    tag_init_var(cursor->var_predicate);
  if (cursor->var_object)
    tag_init_var(cursor->var_object);
  facts_cursor_lock_unlock(cursor);
  return NULL;
}
