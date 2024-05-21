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
#include <stdlib.h>
#include "assert.h"
#include "fact_action.h"
#include "facts.h"
#include "facts_transaction.h"
#include "list.h"

s_facts_transaction * facts_transaction_clean
(s_facts_transaction *transaction)
{
  assert(transaction);
  fact_action_delete_all(transaction->log);
  return transaction->next;
}

s_facts * facts_transaction_rollback
(s_facts *facts, const s_facts_transaction *transaction)
{
  bool b;
  s_fact_action *log;
  const s_facts_transaction *t;
  t = facts->transaction;
  while (t) {
    if (t == transaction)
      goto rollback;
    t = t->next;
  }
  err_puts("facts_transaction_rollback: transaction not found");
  assert(! "facts_transaction_rollback: transaction not found");
  return NULL;
 rollback:
  while (t) {
    log = t->log;
    while (log) {
      if (log->remove) {
        if (! facts_add_fact(facts, &log->fact))
          return NULL;
      }
      else {
        if (! facts_remove_fact(facts, &log->fact, &b) ||
            ! b)
          return NULL;
      }
      log = log->next;
    }
    if (t == transaction)
      return facts;
    t = t->next;
  }
  err_puts("facts_transaction_rollback: unknown error");
  assert(! "facts_transaction_rollback: unknown error");
  exit(1);
  return NULL;
}

void facts_transaction_start (s_facts *facts,
                              s_facts_transaction *transaction)
{
  assert(facts);
  assert(transaction);
  transaction->log = NULL;
  transaction->next = facts->transaction;
  facts->transaction = transaction;
}
