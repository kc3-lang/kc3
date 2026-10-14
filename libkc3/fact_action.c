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
#include "fact.h"
#include "fact_action.h"
#include "facts.h"

void fact_action_delete_all (s_fact_action *action)
{
  s_fact_action *next;
  while (action) {
    next = action->next;
    facts_unref_tag(action->facts, action->fact.subject);
    facts_unref_tag(action->facts, action->fact.predicate);
    facts_unref_tag(action->facts, action->fact.object);
    if (action->remove_log) {
      fact_w_clean(&action->remove_log->fact);
      alloc_free(action->remove_log);
    }
    alloc_free(action);
    action = next;
  }
}

s_fact_action * fact_action_new (s_facts *facts, e_fact_action action,
                                 const s_fact *fact, uw id,
                                 bool broadcast)
{
  s_fact_action *tmp;
  if (! (tmp = alloc(sizeof(s_fact_action))))
    return NULL;
  tmp->action = action;
  tmp->facts = facts;
  tmp->id = id;
  tmp->broadcast = broadcast;
  tmp->fact.id = fact->id;
  if (! (tmp->fact.subject = facts_ref_tag(facts, fact->subject)))
    goto ko;
  if (! (tmp->fact.predicate = facts_ref_tag(facts, fact->predicate)))
    goto clean_subject;
  if (! (tmp->fact.object = facts_ref_tag(facts, fact->object)))
    goto clean_predicate;
  if (action == FACT_ACTION_REMOVE && facts->server_count) {
    if (! (tmp->remove_log = alloc(sizeof(s_facts_remove_log))))
      goto clean_object;
    if (! fact_w_init_fact(&tmp->remove_log->fact, &tmp->fact)) {
      alloc_free(tmp->remove_log);
      goto clean_object;
    }
    tmp->remove_log->id = id;
  }
  return tmp;
 clean_object:
  facts_unref_tag(facts, tmp->fact.object);
 clean_predicate:
  facts_unref_tag(facts, tmp->fact.predicate);
 clean_subject:
  facts_unref_tag(facts, tmp->fact.subject);
 ko:
  alloc_free(tmp);
  return NULL;
}
