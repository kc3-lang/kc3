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
#ifndef LIBKC3_FACTS_CURSOR_H
#define LIBKC3_FACTS_CURSOR_H

#include "types.h"

/* Stack allocation compatible functions */
void             facts_cursor_clean (s_facts_cursor *cursor);
s_facts_cursor * facts_cursor_init (s_facts *facts,
                                    s_facts_cursor *cursor,
                                    s_skiplist__fact *tree,
                                    s_fact *start,
                                    s_fact *end);

/* Modifiers */
#if HAVE_PTHREAD
s_facts_cursor * facts_cursor_lock (s_facts_cursor *cursor);
s_facts_cursor * facts_cursor_lock_clean (s_facts_cursor *cursor);
s_facts_cursor * facts_cursor_lock_init (s_facts_cursor *cursor);
s_facts_cursor * facts_cursor_lock_unlock (s_facts_cursor *cursor);
#endif
s_fact **        facts_cursor_next (s_facts_cursor *cursor,
                                    s_fact **dest);

#endif /* LIBKC3_FACTS_H */
