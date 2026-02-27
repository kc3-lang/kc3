/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "list.h"
#include "mutex.h"
#include "pvar.h"
#include "sym.h"
#include "tag.h"
#include "var.h"

s_facts_with_cursor * facts_with (s_facts *facts,
                                  s_facts_with_cursor *cursor,
                                  p_facts_spec spec)
{
  uw facts_count;
  assert(facts);
  assert(cursor);
  assert(spec);
  facts_count = facts_spec_count_facts(spec);
  *cursor = (s_facts_with_cursor) {0};
  cursor->facts = facts;
  cursor->facts_count = facts_count;
  if (facts_count > 0) {
    cursor->levels = alloc(facts_count *
                           sizeof(s_facts_with_cursor_level));
    if (! cursor->levels)
      return NULL;
    cursor->spec = facts_spec_new_expand(spec);
    if (false) {
      err_write_1("facts_with: spec = ");
      err_inspect_facts_spec(spec);
      err_write_1("\nfacts_with: cursor->spec = ");
      err_inspect_facts_spec(cursor->spec);
      err_write_1("\n");
    }
    /* facts_spec_sort(cursor->spec); */
  }
#if HAVE_PTHREAD
  mutex_init(&cursor->mutex);
#endif
  return cursor;
}

s_facts_cursor * facts_with_0 (s_facts *facts,
                               s_facts_cursor *cursor,
                               p_var pvar_subject,
                               p_var pvar_predicate,
                               p_var pvar_object)
{
  assert(facts);
  assert(cursor);
  facts_cursor_init(facts, cursor, facts->index_spo, NULL, NULL);
  pvar_init_copy(&cursor->pvar_subject, &pvar_subject);
  pvar_init_copy(&cursor->pvar_predicate, &pvar_predicate);
  pvar_init_copy(&cursor->pvar_object, &pvar_object);
  return cursor;
}

s_facts_cursor * facts_with_0_id (s_facts *facts,
                                  s_facts_cursor *cursor,
                                  p_var pvar_subject,
                                  p_var pvar_predicate,
                                  p_var pvar_object)
{
  assert(facts);
  assert(cursor);
  facts_cursor_init(facts, cursor, facts->index, NULL, NULL);
  pvar_init_copy(&cursor->pvar_subject, &pvar_subject);
  pvar_init_copy(&cursor->pvar_predicate, &pvar_predicate);
  pvar_init_copy(&cursor->pvar_object, &pvar_object);
  return cursor;
}

s_facts_cursor * facts_with_1_2 (s_facts *facts,
                                 s_facts_cursor *cursor,
                                 s_tag *subject,
                                 s_tag *predicate,
                                 s_tag *object,
                                 p_var pvar_subject,
                                 p_var pvar_predicate,
                                 p_var pvar_object)
{
  s_fact start;
  s_fact end;
  s_skiplist__fact *tree;
  assert(facts);
  assert(cursor);
  assert(subject);
  assert(predicate);
  assert(object);
  assert(pvar_subject || pvar_predicate || pvar_object);
  start.subject   = pvar_subject   ? TAG_FIRST : subject;
  start.predicate = pvar_predicate ? TAG_FIRST : predicate;
  start.object    = pvar_object    ? TAG_FIRST : object;
  while (start.subject->type == TAG_PVAR &&
         start.subject->data.td_pvar->bound)
    start.subject = &start.subject->data.td_pvar->tag;
  while (start.predicate->type == TAG_PVAR &&
         start.predicate->data.td_pvar->bound)
    start.predicate = &start.predicate->data.td_pvar->tag;
  while (start.object->type == TAG_PVAR &&
         start.object->data.td_pvar->bound)
    start.object = &start.object->data.td_pvar->tag;
  end.subject   = pvar_subject   ? TAG_LAST : subject;
  end.predicate = pvar_predicate ? TAG_LAST : predicate;
  end.object    = pvar_object    ? TAG_LAST : object;
  while (end.subject->type == TAG_PVAR &&
         end.subject->data.td_pvar->bound)
    end.subject = &end.subject->data.td_pvar->tag;
  while (end.predicate->type == TAG_PVAR &&
         end.predicate->data.td_pvar->bound)
    end.predicate = &end.predicate->data.td_pvar->tag;
  while (end.object->type == TAG_PVAR &&
         end.object->data.td_pvar->bound)
    end.object = &end.object->data.td_pvar->tag;
  if (! pvar_subject && pvar_object)
    tree = facts->index_spo;
  else if (! pvar_predicate)
    tree = facts->index_pos;
  else
    tree = facts->index_osp;
  facts_cursor_init(facts, cursor, tree, &start, &end);
  if (pvar_subject)
    pvar_init_copy(&cursor->pvar_subject, &pvar_subject);
  if (pvar_predicate)
    pvar_init_copy(&cursor->pvar_predicate, &pvar_predicate);
  if (pvar_object)
    pvar_init_copy(&cursor->pvar_object, &pvar_object);
  return cursor;
}

s_facts_cursor * facts_with_3 (s_facts *facts,
                               s_facts_cursor *cursor,
                               s_tag *subject,
                               s_tag *predicate,
                               s_tag *object)
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
  return facts_cursor_init(facts, cursor, facts->index_spo,
                           &fact, &fact);
}

s_facts_with_cursor * facts_with_list (s_facts *facts,
                                       s_facts_with_cursor *cursor,
                                       s_list *spec)
{
  p_facts_spec facts_spec = NULL;
  assert(facts);
  assert(cursor);
  assert(spec);
  if (! (facts_spec = facts_spec_new_list(spec))) {
    err_puts("facts_with_list: facts_spec_new_list");
    assert(! "facts_with_list: facts_spec_new_list");
    goto ko;
  }
  if (false) {
    err_write_1("facts_with_list: spec = ");
    err_inspect_facts_spec(facts_spec);
    err_write_1("\n");
  }
  if (! facts_with(facts, cursor, facts_spec))
    goto ko;
  alloc_free(facts_spec);
  return cursor;
 ko:
  alloc_free(facts_spec);
  return NULL;
}

s_facts_cursor * facts_with_tags (s_facts *facts,
                                  s_facts_cursor *cursor,
                                  s_tag *subject,
                                  s_tag *predicate,
                                  s_tag *object)
{
  bool unbound;
  p_var pvar_subject = NULL;
  p_var pvar_predicate = NULL;
  p_var pvar_object = NULL;
  assert(facts);
  assert(cursor);
  assert(subject);
  assert(predicate);
  assert(object);
  if (! tag_is_unbound_var(subject, &unbound))
    return NULL;
  if (unbound)
    pvar_subject = subject->data.td_pvar;
  if (! tag_is_unbound_var(predicate, &unbound))
    return NULL;
  if (unbound)
    pvar_predicate = predicate->data.td_pvar;
  if (! tag_is_unbound_var(object, &unbound))
    return NULL;
  if (unbound)
    pvar_object = object->data.td_pvar;
  if (pvar_subject && pvar_predicate && pvar_object)
    return facts_with_0(facts, cursor, pvar_subject, pvar_predicate,
                        pvar_object);
  if (! (pvar_subject || pvar_predicate || pvar_object))
    return facts_with_3(facts, cursor, subject, predicate, object);
  return facts_with_1_2(facts, cursor, subject, predicate, object,
                        pvar_subject, pvar_predicate, pvar_object);
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

