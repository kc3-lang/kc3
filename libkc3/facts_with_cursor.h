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
#ifndef LIBKC3_FACTS_WITH_CURSOR_H
#define LIBKC3_FACTS_WITH_CURSOR_H

#include "types.h"

void facts_with_cursor_clean (s_facts_with_cursor *cursor);
s_fact ** facts_with_cursor_next (s_facts_with_cursor *cursor,
                                  s_fact **dest);

#endif /* LIBKC3_FACTS_WITH_CURSOR_H */
