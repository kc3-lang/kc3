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
#ifndef LIBKC3_ENV_EVAL_EQUAL_H
#define LIBKC3_ENV_EVAL_EQUAL_H

#include "types.h"

/* Evaluator as a code walker for operator = equal. */
bool env_eval_equal_cow (s_env *env, bool macro, s_cow *a,
                         s_cow *b, s_cow **dest);
bool env_eval_equal_do_block (s_env *env, bool macro,
                              s_do_block *a, s_do_block *b,
                              s_do_block *dest);
bool env_eval_equal_list (s_env *env, bool macro,
                          s_list *a, s_list *b,
                          s_list **dest);
bool env_eval_equal_struct (s_env *env, bool macro,
                            s_struct *a,
                            s_struct *b,
                            p_struct *dest);
bool env_eval_equal_tag (s_env *env, bool macro,
                         s_tag *a, s_tag *b,
                         s_tag *dest);
bool env_eval_equal_time (s_env *env, bool macro,
                          s_time *a, s_time *b,
                          s_time *dest);
bool env_eval_equal_ptuple (s_env *env, bool macro,
                            p_tuple *a, p_tuple *b,
                            p_tuple *dest);

#endif /* LIBKC3_ENV_EVAL_EQUAL_H */
