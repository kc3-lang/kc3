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
#ifndef LIBKC3_FACTS_H
#define LIBKC3_FACTS_H

#include "types.h"

#define FACTS_EXT ".facts"

#define facts_count(f) ((f)->facts.count)

/* Stack allocation compatible functions */
void      facts_clean (s_facts *facts);
s_facts * facts_init (s_facts *facts);

/* Constructors */
s_facts * facts_new (void);
s_facts * facts_new_ref (s_facts *src);

/* Destructor */
void     facts_delete (s_facts *facts);

/* Modifiers */
s_fact *  facts_add_fact (s_facts *facts, s_fact *fact);
s_fact *  facts_add_fact_id (s_facts *facts, s_fact *fact);
s_fact *  facts_add_fact_local (s_facts *facts, s_fact *fact);
uw *      facts_next_id (s_facts *facts, uw *dest);
s_fact *  facts_add_tags (s_facts *facts, s_tag *subject,
                          s_tag *predicate, s_tag *object);
void      facts_close (s_facts *facts);
sw        facts_load (s_facts *facts, s_buf *buf,
                      const s_str *path);
sw        facts_load_file (s_facts *facts, const s_str *path);
sw        facts_open_file (s_facts *facts, const s_str *path);
sw        facts_open_file_binary (s_facts *facts, const s_str *path);
sw        facts_open_file_binary_create (s_facts *facts,
                                         const s_str *path);
s_tag *   facts_ref_tag (s_facts *facts, s_tag *tag);
s_facts * facts_set_priority (s_facts *facts, u8 priority);
s_facts * facts_set_secret (s_facts *facts, const s_str *secret);
bool *    facts_remove_fact (s_facts *facts, const s_fact *fact,
                             bool *dest);
bool *    facts_remove_fact_local (s_facts *facts, const s_fact *fact,
                                   bool *dest);
bool *    facts_remove_fact_tags (s_facts *facts,
                                  s_tag *subject,
                                  s_tag *predicate,
                                  s_tag *object,
                                  bool *dest);
s_facts * facts_remove_all (s_facts *facts);
s_fact *  facts_replace_fact (s_facts *facts, s_fact *fact);
s_fact *  facts_replace_tags (s_facts *facts, s_tag *subject,
                              s_tag *predicate, s_tag *object);
sw        facts_save_file (s_facts *facts, const s_str *path);
bool      facts_unref_tag (s_facts *facts, const s_tag *tag);

/* Observers */
sw        facts_dump (s_facts *facts, s_buf *buf);
sw        facts_dump_file (s_facts *facts, const s_str *path);
s_fact ** facts_find_fact (s_facts *facts, const s_fact *fact,
                           s_fact **dest);
s_fact ** facts_find_fact_by_tags (s_facts *facts,
                                   s_tag *subject,
                                   s_tag *predicate,
                                   s_tag *object,
                                   s_fact **dest);
s_tag **  facts_find_tag (s_facts *facts, const s_tag *tag,
                          s_tag **dest);
sw        facts_log_add (s_log *log, uw id, const s_fact *fact);
sw        facts_log_remove (s_log *log, uw id, const s_fact *fact);

/* Network. */
bool facts_send_to_master_add (s_facts *facts, const s_fact *fact);
bool facts_send_to_master_remove (s_facts *facts, const s_fact *fact);

#endif /* LIBKC3_FACTS_H */
