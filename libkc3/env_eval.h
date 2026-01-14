/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_ENV_EVAL_H
#define LIBKC3_ENV_EVAL_H

#include "types.h"

/* Evaluator. */
bool env_eval_array (s_env *env, const s_array *array,
                     s_array *dest);
bool env_eval_array_tag (s_env *env, const s_array *array,
                         s_tag *dest);
bool env_eval_do_block (s_env *env, const s_do_block *do_block,
                     s_tag *dest);
bool env_eval_call (s_env *env, s_call *call, s_tag *dest);
bool env_eval_call_arguments (s_env *env, s_list *args,
                              s_list **dest);
bool env_eval_call_callable (s_env *env, s_call *call,
                             s_tag *dest);
bool env_eval_call_callable_args (s_env *env,
                                  s_callable *callable,
                                  s_list *arguments,
                                  s_tag *dest);
bool env_eval_call_cfn_args (s_env *env, s_cfn *cfn, s_list *arguments,
                             s_tag *dest);
bool env_eval_call_fn (s_env *env, const s_call *call,
                       s_tag *dest);
bool env_eval_call_fn_args (s_env *env, const s_fn *fn,
                            s_list *arguments,
                            s_tag *dest);
bool env_eval_call_resolve (s_env *env, s_call *call);
bool env_eval_callable (s_env *env, s_callable *callable,
                        s_tag *dest);
bool env_eval_complex (s_env *env, s_complex *c,
                       s_tag *dest);
bool env_eval_ident (s_env *env, const s_ident *ident,
                     s_tag *dest);
bool env_eval_ident_is_bound (s_env *env,
                              const s_ident *ident);
bool env_eval_integer (s_env *env, const s_integer *integer,
                       s_integer *dest);
bool env_eval_list (s_env *env, s_list *list, s_tag *dest);
bool env_eval_map (s_env *env, s_map *map,
                   s_tag *dest);
bool env_eval_pcow (s_env *env, p_cow *cow, p_cow *dest);
bool env_eval_pcow_tag (s_env *env, p_cow *cow, s_tag *dest);
bool env_eval_progn (s_env *env, s_list *program,
                     s_tag *dest);
bool env_eval_struct (s_env *env, s_struct *s, p_struct *dest);
bool env_eval_struct_tag (s_env *env, s_struct *s, s_tag *dest);
bool env_eval_tag (s_env *env, s_tag *tag, s_tag *dest);
bool env_eval_time (s_env *env, const s_time *time,
                    s_tag *dest);
bool env_eval_tuple (s_env *env, const s_tuple *tuple,
                     s_tag *dest);
bool env_eval_var (s_env *env, s_var *var, s_tag *dest);

#endif /* LIBKC3_ENV_EVAL_H */
