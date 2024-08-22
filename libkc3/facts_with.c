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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "env.h"
#include "facts_cursor.h"
#include "facts_spec.h"
#include "facts_with.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

s_facts_with_cursor * facts_with (s_facts *facts,
                                  s_facts_with_cursor *cursor,
                                  p_facts_spec spec)
{
  uw facts_count;
  s_facts_with_cursor tmp = {0};
  assert(facts);
  assert(cursor);
  assert(spec);
  facts_count = facts_spec_count_facts(spec);
  tmp.facts = facts;
  tmp.facts_count = facts_count;
  if (facts_count > 0) {
    tmp.levels = alloc(facts_count *
                       sizeof(s_facts_with_cursor_level));
    if (! tmp.levels)
      return NULL;
    tmp.spec = facts_spec_new_expand(spec);
    if (false) {
      err_write_1("facts_with: spec = ");
      err_inspect_facts_spec(spec);
      err_write_1("\nfacts_with: tmp.spec = ");
      err_inspect_facts_spec(tmp.spec);
      err_write_1("\n");
    }
    /* facts_spec_sort(tmp.spec); */
  }
  if (pthread_mutex_init(&tmp.mutex, NULL)) {
    err_puts("facts_with: pthread_mutex_init");
    assert(! "facts_with: pthread_mutex_init");
    return NULL;
  }
  *cursor = tmp;
  return cursor;
}

s_facts_cursor * facts_with_0 (s_facts *facts,
                               s_facts_cursor *cursor,
                               s_var *var_subject,
                               s_var *var_predicate,
                               s_var *var_object)
{
  assert(facts);
  assert(cursor);
  facts_cursor_init(facts, cursor, facts->index_spo, NULL, NULL);
  var_init_copy(&cursor->var_subject, var_subject);
  var_init_copy(&cursor->var_predicate, var_predicate);
  var_init_copy(&cursor->var_object, var_object);
  return cursor;
}

s_facts_cursor * facts_with_1_2 (s_facts *facts,
                                 s_facts_cursor *cursor,
                                 const s_tag *subject,
                                 const s_tag *predicate,
                                 const s_tag *object,
                                 s_var *var_subject,
                                 s_var *var_predicate,
                                 s_var *var_object)
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
  if (var_subject)
    var_init_copy(&cursor->var_subject, var_subject);
  if (var_predicate)
    var_init_copy(&cursor->var_predicate, var_predicate);
  if (var_object)
    var_init_copy(&cursor->var_object, var_object);
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
  s_var *var_subject = NULL;
  s_var *var_predicate = NULL;
  s_var *var_object = NULL;
  assert(facts);
  assert(cursor);
  assert(subject);
  assert(predicate);
  assert(object);
  if (! tag_is_unbound_var(subject, &unbound))
    return NULL;
  if (unbound)
    var_subject = &subject->data.var;
  if (! tag_is_unbound_var(predicate, &unbound))
    return NULL;
  if (unbound)
    var_predicate = &predicate->data.var;
  if (! tag_is_unbound_var(object, &unbound))
    return NULL;
  if (unbound)
    var_object = &object->data.var;
  if (var_subject && var_predicate && var_object)
    return facts_with_0(facts, cursor, var_subject, var_predicate,
                        var_object);
  if (! (var_subject || var_predicate || var_object))
    return facts_with_3(facts, cursor, subject, predicate, object);
  return facts_with_1_2(facts, cursor, subject, predicate, object,
                        var_subject, var_predicate, var_object);
}

s_facts_cursor * facts_with_tuple (s_facts *facts,
                                   s_facts_cursor *cursor,
                                   s_tuple *fact)
{
  assert(facts);
  assert(cursor);
  assert(fact);
  if (fact->count != 3) {
    err_write_1("facts_with_tuple: Tuple count != 3: ");
    err_inspect_tuple(fact);
    err_write_1("\n");
    assert(! "facts_with_tuple: Tuple count != 3");
    return NULL;
  }
  return facts_with_tags(facts, cursor, fact->tag, fact->tag + 1,
                         fact->tag + 2);
}

