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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "env.h"
#include "facts_cursor.h"
#include "facts_spec.h"
#include "facts_with.h"
#include "tag.h"

s_facts_with_cursor * facts_with (s_facts *facts,
                                  s_facts_with_cursor *cursor,
                                  p_facts_spec spec)
{
  uw facts_count;
  assert(facts);
  assert(cursor);
  assert(spec);
  facts_count = facts_spec_count_facts(spec);
  cursor->facts = facts;
  cursor->facts_count = facts_count;
  if (facts_count > 0) {
    cursor->levels = alloc(facts_count *
                           sizeof(s_facts_with_cursor_level));
    if (! cursor->levels)
      return NULL;
    cursor->spec = facts_spec_new_expand(spec);
    /*
    buf_inspect_facts_spec(&g_c3_env.err, spec);
    buf_write_1(&g_c3_env.err, "\n");
    buf_inspect_facts_spec(&g_c3_env.err, cursor->spec);
    buf_write_1(&g_c3_env.err, "\n");
    buf_flush(&g_c3_env.err);
    */
    /* facts_spec_sort(cursor->spec); */
  }
  else {
    cursor->levels = NULL;
    cursor->spec = NULL;
  }
  cursor->level = 0;
  if (pthread_mutex_init(&cursor->mutex, NULL)) {
    err_puts("facts_with: pthread_mutex_init");
    assert(! "facts_with: pthread_mutex_init");
    return NULL;
  }
  return cursor;
}

s_facts_cursor * facts_with_0 (s_facts *facts,
                               s_facts_cursor *cursor,
                               s_tag *var_subject,
                               s_tag *var_predicate,
                               s_tag *var_object)
{
  assert(facts);
  assert(cursor);
  facts_cursor_init(facts, cursor, facts->index_spo, NULL, NULL);
  cursor->var_subject = var_subject;
  cursor->var_subject_type =
    var_subject ? var_subject->data.var.type : NULL;
  cursor->var_predicate = var_predicate;
  cursor->var_predicate_type =
    var_predicate ? var_predicate->data.var.type : NULL;
  cursor->var_object = var_object;
  cursor->var_object_type =
    var_object ? var_object->data.var.type : NULL;
  return cursor;
}

s_facts_cursor * facts_with_1_2 (s_facts *facts,
                                 s_facts_cursor *cursor,
                                 const s_tag *subject,
                                 const s_tag *predicate,
                                 const s_tag *object,
                                 s_tag *var_subject,
                                 s_tag *var_predicate,
                                 s_tag *var_object)
{
  s_fact start;
  s_fact end;
  s_skiplist__fact *tree;
  assert(facts);
  assert(cursor);
  assert(subject);
  assert(predicate);
  assert(object);
  assert(var_subject || var_predicate || var_object);
  start.subject   = var_subject   ? TAG_FIRST : subject;
  start.predicate = var_predicate ? TAG_FIRST : predicate;
  start.object    = var_object    ? TAG_FIRST : object;
  end.subject   = var_subject   ? TAG_LAST : subject;
  end.predicate = var_predicate ? TAG_LAST : predicate;
  end.object    = var_object    ? TAG_LAST : object;
  if (! var_subject && var_object)
    tree = facts->index_spo;
  else if (! var_predicate)
    tree = facts->index_pos;
  else
    tree = facts->index_osp;
  facts_cursor_init(facts, cursor, tree, &start, &end);
  cursor->var_subject = var_subject;
  cursor->var_subject_type =
    var_subject ? var_subject->data.var.type : NULL;
  cursor->var_predicate = var_predicate;
  cursor->var_predicate_type =
    var_predicate ? var_predicate->data.var.type : NULL;
  cursor->var_object = var_object;
  cursor->var_object_type =
    var_object ? var_object->data.var.type : NULL;
  return cursor;
}

s_facts_cursor * facts_with_3 (s_facts *facts,
                               s_facts_cursor *cursor,
                               const s_tag *subject,
                               const s_tag *predicate,
                               const s_tag *object)
{
  s_fact fact;
  assert(facts);
  assert(cursor);
  assert(subject);
  assert(predicate);
  assert(object);
  fact.subject = subject;
  fact.predicate = predicate;
  fact.object = object;
  return facts_cursor_init(facts, cursor, facts->index_spo, &fact, &fact);
}

s_facts_cursor * facts_with_tags (s_facts *facts,
                                  s_facts_cursor *cursor,
                                  s_tag *subject,
                                  s_tag *predicate,
                                  s_tag *object)
{
  bool unbound;
  s_tag *var_subject = NULL;
  s_tag *var_predicate = NULL;
  s_tag *var_object = NULL;
  assert(facts);
  assert(cursor);
  assert(subject);
  assert(predicate);
  assert(object);
  if (! tag_is_unbound_var(subject, &unbound))
    return NULL;
  if (unbound)
    var_subject = subject;
  if (! tag_is_unbound_var(predicate, &unbound))
    return NULL;
  if (unbound)
    var_predicate = predicate;
  if (! tag_is_unbound_var(object, &unbound))
    return NULL;
  if (unbound)
    var_object = object;
  if (var_subject && var_predicate && var_object)
    return facts_with_0(facts, cursor, var_subject, var_predicate,
                        var_object);
  if (! (var_subject || var_predicate || var_object))
    return facts_with_3(facts, cursor, subject, predicate, object);
  return facts_with_1_2(facts, cursor, subject, predicate, object,
                        var_subject, var_predicate, var_object);
}
