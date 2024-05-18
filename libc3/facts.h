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
#ifndef LIBC3_FACTS_H
#define LIBC3_FACTS_H

#include "types.h"

#define FACTS_EXT ".facts"

#define facts_count(f) ((f)->facts.count)

/* Stack allocation compatible functions */
void      facts_clean (s_facts *facts);
s_facts * facts_init (s_facts *facts);

/* Constructors */
s_facts * facts_new (void);

/* Destructor */
void     facts_delete (s_facts *facts);

/* Modifiers */
s_fact *  facts_add_fact (s_facts *facts, const s_fact *fact);
s_fact *  facts_add_tags (s_facts *facts, const s_tag *subject,
                          const s_tag *predicate,
                          const s_tag *object);
void      facts_close (s_facts *facts);
sw        facts_load (s_facts *facts, s_buf *buf, const s_str *path);
sw        facts_load_file (s_facts *facts, const s_str *path);
s_facts * facts_lock_clean (s_facts *facts);
s_facts * facts_lock_init (s_facts *facts);
s_facts * facts_lock_r (s_facts *facts);
s_facts * facts_lock_unlock_r (s_facts *facts);
s_facts * facts_lock_unlock_w (s_facts *facts);
s_facts * facts_lock_w (s_facts *facts);
sw        facts_open_file (s_facts *facts, const s_str *path);
s_tag *   facts_ref_tag (s_facts *facts, const s_tag *tag);
bool      facts_remove_fact (s_facts *facts, const s_fact *fact);
bool      facts_remove_fact_tags (s_facts *facts, const s_tag *subject,
                                  const s_tag *predicate,
                                  const s_tag *object);
void      facts_remove_all (s_facts *facts);
s_fact *  facts_replace_fact (s_facts *facts, const s_fact *fact);
s_fact *  facts_replace_tags (s_facts *facts, const s_tag *subject,
                              const s_tag *predicate,
                              const s_tag *object);
sw        facts_save_file (s_facts *facts, const char *path);
s_facts * facts_transaction_rollback (s_facts *facts,
                                      s_facts_transaction *transaction);
void      facts_transaction_start (s_facts *facts,
                                   s_facts_transaction *transaction);
bool      facts_unref_tag (s_facts *facts, const s_tag *tag);

/* Observers */
sw       facts_dump (s_facts *facts, s_buf *buf);
sw       facts_dump_file (s_facts *facts, const char *path);
s_fact * facts_find_fact (s_facts *facts, const s_fact *fact);
s_fact * facts_find_fact_by_tags (s_facts *facts, const s_tag *subject,
                                  const s_tag *predicate,
                                  const s_tag *object);
s_tag *  facts_find_tag (s_facts *facts, const s_tag *tag);
sw       facts_log_add (s_log *log, const s_fact *fact);
sw       facts_log_remove (s_log *log, const s_fact *fact);

#endif /* LIBC3_FACTS_H */
