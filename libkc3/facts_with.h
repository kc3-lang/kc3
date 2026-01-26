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
#ifndef LIBKC3_FACTS_WITH_H
#define LIBKC3_FACTS_WITH_H

#include "types.h"

s_facts_with_cursor * facts_with (s_facts *facts,
                                  s_facts_with_cursor *cursor,
                                  p_facts_spec spec);

s_facts_cursor *      facts_with_0 (s_facts *facts,
                                    s_facts_cursor *cursor,
                                    s_var *var_subject,
                                    s_var *var_predicate,
                                    s_var *var_object);

s_facts_cursor *      facts_with_0_id (s_facts *facts,
                                       s_facts_cursor *cursor,
                                       s_var *var_subject,
                                       s_var *var_predicate,
                                       s_var *var_object);

s_facts_cursor *      facts_with_1_2 (s_facts *facts,
                                      s_facts_cursor *cursor,
                                      s_tag *subject,
                                      s_tag *predicate,
                                      s_tag *object,
                                      s_var *var_subject,
                                      s_var *var_predicate,
                                      s_var *var_object);

s_facts_cursor *      facts_with_3 (s_facts *facts,
                                    s_facts_cursor *cursor,
                                    s_tag *subject,
                                    s_tag *predicate,
                                    s_tag *object);

s_facts_with_cursor * facts_with_list (s_facts *facts,
                                       s_facts_with_cursor *cursor,
                                       s_list *spec);

s_facts_cursor *      facts_with_tags (s_facts *facts,
                                       s_facts_cursor *cursor,
                                       s_tag *subject,
                                       s_tag *predicate,
                                       s_tag *object);

#endif /* LIBKC3_FACTS_WITH_H */
