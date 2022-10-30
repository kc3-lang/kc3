/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "facts_cursor.h"
#include "skiplist__fact.h"
#include "skiplist_node__fact.h"
#include "tag.h"

s_facts_cursor * facts_cursor_init (s_facts_cursor *cursor,
                                    s_skiplist__fact *tree,
                                    s_fact *start,
                                    s_fact *end)
{
  s_skiplist_node__fact *pred;
  assert(cursor);
  assert(tree);
  pred = skiplist_pred__fact(tree, start);
  assert(pred);
  cursor->tree = tree;
  cursor->node = SKIPLIST_NODE_NEXT__fact(pred, 0);
  skiplist_node_delete__fact(pred);
  if (start)
    cursor->start = *start;
  else {
    cursor->start.subject =
      cursor->start.predicate =
      cursor->start.object = TAG_FIRST;
  }
  if (end)
    cursor->end = *end;
  else {
    cursor->end.subject =
      cursor->end.predicate =
      cursor->end.object = TAG_LAST;
  }
  cursor->var_subject = NULL;
  cursor->var_predicate = NULL;
  cursor->var_object = NULL;
  return cursor;
}

s_fact * facts_cursor_next (s_facts_cursor *cursor)
{
  assert(cursor);
  if (cursor->node) {
    cursor->node = SKIPLIST_NODE_NEXT__fact(cursor->node, 0);
    if (cursor->node &&
        cursor->tree->compare(&cursor->end, cursor->node->fact) < 0)
      cursor->node = NULL;
  }
  if (cursor->node) {
    s_fact *fact = cursor->node->fact;
    if (cursor->var_subject)
      tag_copy(fact->subject, cursor->var_subject);
    if (cursor->var_predicate)
      tag_copy(fact->predicate, cursor->var_predicate);
    if (cursor->var_object)
      tag_copy(fact->object, cursor->var_object);
    return fact;
  }
  if (cursor->var_subject)
    tag_init_var(cursor->var_subject);
  if (cursor->var_predicate)
    tag_init_var(cursor->var_predicate);
  if (cursor->var_object)
    tag_init_var(cursor->var_object);
  return NULL;
}
