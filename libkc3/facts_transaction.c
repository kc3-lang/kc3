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
#include "fact.h"
#include "fact_action.h"
#include "facts.h"
#include "facts_transaction.h"
#include "list.h"
#include "rwlock.h"

s_facts_transaction * facts_transaction_clean
(s_facts_transaction *transaction)
{
  assert(transaction);
  fact_action_delete_all(transaction->log);
  return transaction->next;
}

s_facts * facts_transaction_end (s_facts *facts,
                                 s_facts_transaction *transaction)
{
  assert(facts);
  assert(transaction);
  if (facts->transaction != transaction) {
    err_puts("facts_transaction_end: transaction mismatch");
    assert(! "facts_transaction_end: transaction mismatch");
    return NULL;
  }
  facts->transaction = facts_transaction_clean(transaction);
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return facts;
}

bool facts_transaction_find (s_facts *facts,
			     const s_facts_transaction *transaction)
{
  const s_facts_transaction *t;
  t = facts->transaction;
  while (t) {
    if (t == transaction)
      return true;
    t = t->next;
  }
  err_puts("facts_transaction_find: transaction not found");
  assert(! "facts_transaction_find: transaction not found");
  return false;
}

s_facts_transaction * facts_transaction_init
(s_facts_transaction *transaction)
{
  s_facts_transaction tmp = {0};
  *transaction = tmp;
  return transaction;
}

s_facts * facts_transaction_rollback
(s_facts *facts, const s_facts_transaction *transaction)
{
  bool b;
  s_fact fact;
  s_fact_action *log;
  s_facts_transaction *t;
  if (! facts_transaction_find(facts, transaction))
    return NULL;
  t = facts->transaction;
  while (t) {
    log = t->log;
    while (log) {
      switch (log->action) {
      case FACT_ACTION_ADD:
        if (! facts_remove_fact(facts, &log->fact, &b) ||
            ! b) {
	  abort();
	  return NULL;
	}
	break;
      case FACT_ACTION_REMOVE:
        if (! facts_add_fact(facts, &log->fact)) {
	  abort();
	  return NULL;
	}
	break;
      case FACT_ACTION_REPLACE:
	fact_init(&fact, log->fact.subject, log->fact.predicate,
		  &log->object);
	if (! facts_replace_fact(facts, &fact)) {
	  abort();
	  return NULL;
	}
      }
      log = log->next;
    }
    if (t == transaction) {
      facts_transaction_end(facts, t);
      return facts;
    }
    facts_transaction_end(facts, t);
    t = facts->transaction;
  }
  err_puts("facts_transaction_rollback: no transaction in facts");
  assert(! "facts_transaction_rollback: no transaction in facts");
  abort();
  return NULL;
}

s_facts * facts_transaction_start (s_facts *facts,
                                   s_facts_transaction *transaction)
{
  assert(facts);
  assert(transaction);
#if HAVE_PTHREAD
  if (! rwlock_w(&facts->rwlock))
    return NULL;
#endif
  if (facts->next_id == UW_MAX) {
    err_puts("facts_next_id: id exhausted");
    assert(! "facts_next_id: id exhausted");
    return NULL;
  }
  transaction->id = facts->next_id++;
  transaction->log = NULL;
  transaction->next = facts->transaction;
  facts->transaction = transaction;
  return facts;
}
