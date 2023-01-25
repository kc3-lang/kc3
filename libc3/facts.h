/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef FACTS_H
#define FACTS_H

#include "types.h"

#define facts_count(f) ((f)->facts.count)

/* Stack allocation compatible functions */
void      facts_clean (s_facts *facts);
s_facts * facts_init (s_facts *facts);

/* Constructors */
s_facts * facts_new ();

/* Destructor */
void     facts_delete (s_facts *facts);

/* Modifiers */
s_fact * facts_add_fact (s_facts *facts, const s_fact *fact);
s_fact * facts_add_tags (s_facts *facts, const s_tag *subject,
                         const s_tag *predicate,
                         const s_tag *object);
void     facts_close (s_facts *facts);
sw       facts_load (s_facts *facts, s_buf *buf);
sw       facts_load_file (s_facts *facts, const s8 *path);
sw       facts_open_file (s_facts *facts, const s8 *path);
s_tag *  facts_ref_tag (s_facts *facts, const s_tag *tag);
e_bool   facts_remove_fact (s_facts *facts, const s_fact *fact);
sw       facts_save_file (s_facts *facts, const s8 *path);
e_bool   facts_unref_tag (s_facts *facts, const s_tag *tag);

/* Observers */
sw       facts_dump (const s_facts *facts, s_buf *buf);
sw       facts_dump_file (const s_facts *facts, const s8 *path);
s_fact * facts_find_fact (const s_facts *facts, const s_fact *fact);
s_tag *  facts_find_tag (const s_facts *facts, const s_tag *tag);
sw       facts_log_add (s_log *log, const s_fact *fact);
sw       facts_log_remove (s_log *log, const s_fact *fact);
sw       facts_save_header (s_buf *buf);

#endif /* FACTS_H */
