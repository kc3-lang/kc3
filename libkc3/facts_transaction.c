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
#include <stdlib.h>
#include "assert.h"
#include "fact.h"
#include "fact_action.h"
#include "facts.h"
#include "facts_connection.h"
#include "facts_transaction.h"
#include "list.h"
#include "rwlock.h"

s_facts_transaction * facts_transaction_clean
(s_facts_transaction *transaction)
{
  s_facts_transaction *next;
  assert(transaction);
  next = transaction->next;
  fact_action_delete_all(transaction->log);
  transaction->log = NULL;
  transaction->next = NULL;
  return next;
}

s_facts * facts_transaction_end (s_facts *facts,
                                 s_facts_transaction *transaction)
{
  s_fact_action *log;
  s_fact_action *next;
  s_fact_action *ordered = NULL;
  bool ok = true;
  assert(facts);
  assert(transaction);
  if (facts->transaction != transaction) {
    err_puts("facts_transaction_end: transaction mismatch");
    assert(! "facts_transaction_end: transaction mismatch");
    return NULL;
  }
  if (transaction->next) {
    if ((log = transaction->log)) {
      while (log->next)
        log = log->next;
      log->next = transaction->next->log;
      transaction->next->log = transaction->log;
      transaction->log = NULL;
    }
  }
  else {
    log = transaction->log;
    while (log) {
      next = log->next;
      log->next = ordered;
      ordered = log;
      log = next;
    }
    transaction->log = ordered;
    facts->transaction = NULL;
    for (log = ordered; ok && log; log = log->next) {
      switch (log->action) {
      case FACT_ACTION_ADD:
        if (facts->log &&
            facts_log_add(facts->log, log->id, &log->fact) < 0) {
          ok = false;
          break;
        }
        if (log->broadcast)
          facts_broadcast_add(facts, &log->fact);
        break;
      case FACT_ACTION_REMOVE:
        if (facts->log &&
            facts_log_remove(facts->log, log->id, &log->fact) < 0) {
          ok = false;
          break;
        }
        if (log->broadcast)
          facts_broadcast_remove(facts, &log->fact);
        if (log->remove_log) {
          log->remove_log->next = facts->remove_log;
          facts->remove_log = log->remove_log;
          log->remove_log = NULL;
        }
        break;
      default:
        abort();
      }
    }
  }
  facts->transaction = facts_transaction_clean(transaction);
#if HAVE_PTHREAD
  rwlock_unlock_w(&facts->rwlock);
#endif
  return ok ? facts : NULL;
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
  s_fact_action *log;
  s_facts_transaction *t;
  if (! facts_transaction_find(facts, transaction))
    return NULL;
  t = facts->transaction;
  while (t) {
    t->rolling_back = true;
    log = t->log;
    while (log) {
      switch (log->action) {
      case FACT_ACTION_ADD:
        if (! facts_remove_fact_local(facts, &log->fact, &b) ||
            ! b) {
	  abort();
	  return NULL;
	}
	break;
      case FACT_ACTION_REMOVE:
        if (! facts_add_fact_id(facts, &log->fact)) {
	  abort();
	  return NULL;
	}
	break;
      default:
        abort();
      }
      log = log->next;
    }
    facts->transaction = facts_transaction_clean(t);
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    if (t == transaction)
      return facts;
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
  if (facts->log && facts->log->failed) {
    err_puts("facts_transaction_start: log failed");
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  if (facts_connection_get_master(facts)) {
    err_puts("facts_transaction_start: remote master is not supported");
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  if (facts->next_id == UW_MAX) {
    err_puts("facts_next_id: id exhausted");
    assert(! "facts_next_id: id exhausted");
#if HAVE_PTHREAD
    rwlock_unlock_w(&facts->rwlock);
#endif
    return NULL;
  }
  transaction->id = facts->next_id++;
  transaction->log = NULL;
  transaction->next = facts->transaction;
  transaction->rolling_back = false;
  facts->transaction = transaction;
  return facts;
}
