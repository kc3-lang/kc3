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
#ifndef LIBC3_FACTS_TRANSACTION_H
#define LIBC3_FACTS_TRANSACTION_H

#include "types.h"

/* Stack allocation compatible functions, call
   facts_transaction_clean after use. */
s_facts_transaction * facts_transaction_clean
(s_facts_transaction *transaction);
s_facts_transaction * facts_transaction_init
(s_facts_transaction *transaction);

/* Operators. */
void      facts_transaction_end
(s_facts *facts, s_facts_transaction *transaction);
s_facts * facts_transaction_rollback
(s_facts *facts, const s_facts_transaction *transaction);
void      facts_transaction_start
(s_facts *facts, s_facts_transaction *transaction);

#endif /* LIBC3_FACTS_TRANSACTION_H */
