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
#ifndef LIBKC3_ENV_EVAL_QUOTE_H
#define LIBKC3_ENV_EVAL_QUOTE_H

#include "types.h"

/* Evaluator as a code walker for quote/unquote. */
bool env_eval_quote (s_env *env, s_quote *quote,
                     s_tag *dest);
bool env_eval_quote_array (s_env *env, s_array *array,
                           s_tag *dest);
bool env_eval_quote_do_block (s_env *env, s_do_block *do_block,
                           s_tag *dest);
bool env_eval_quote_call (s_env *env, s_call *call, s_tag *dest);
bool env_eval_quote_complex (s_env *env, s_complex *c,
                             s_tag *dest);
bool env_eval_quote_cow (s_env *env, s_cow *cow,
                         s_tag *dest);
bool env_eval_quote_list (s_env *env, s_list *list,
                          s_tag *dest);
bool env_eval_quote_map (s_env *env, s_map *map,
                         s_tag *dest);
bool env_eval_quote_quote (s_env *env, s_quote *quote,
                           s_tag *dest);
bool env_eval_quote_struct (s_env *env, s_struct *s,
                            s_tag *dest);
bool env_eval_quote_tag (s_env *env, s_tag *tag,
                         s_tag *dest);
bool env_eval_quote_time (s_env *env, s_time *time,
                          s_tag *dest);
bool env_eval_quote_tuple (s_env *env, s_tuple *tuple,
                           s_tag *dest);
bool env_eval_quote_unquote (s_env *env,
                             s_unquote *unquote,
                             s_tag *dest);

#endif /* LIBKC3_ENV_EVAL_QUOTE_H */
