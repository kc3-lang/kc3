/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef FACTS_H
#define FACTS_H

#include "types.h"

/* Stack allocation compatible functions */
void      facts_clean (s_facts *facts);
s_facts * facts_init (s_facts *facts, s_buf *log);

/* Constructors */
s_facts * facts_new (s_buf *log);

/* Destructor */
void     facts_delete (s_facts *facts);

/* Modifiers */
s_fact * facts_add_fact (s_facts *facts, const s_fact *fact);
s_fact * facts_add_tags (s_facts *facts, const s_tag *subject,
                         const s_tag *predicate,
                         const s_tag *object);
sw       facts_load (s_facts *facts, const s8 *path);
s_tag *  facts_ref_tag (s_facts *facts, const s_tag *tag);
e_bool   facts_remove_fact (s_facts *facts, const s_fact *fact);
e_bool   facts_unref_tag (s_facts *facts, const s_tag *tag);

/* Observers */
sw       facts_dump (const s_facts *facts, const s8 *path);
s_fact * facts_find_fact (const s_facts *facts, const s_fact *fact);
s_tag *  facts_find_tag (const s_facts *facts, const s_tag *tag);
sw       facts_log_add (s_buf *log, const s_fact *fact);
sw       facts_log_remove (s_buf *log, const s_fact *fact);

#endif /* FACTS_H */
