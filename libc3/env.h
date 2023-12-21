/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_ENV_H
#define LIBC3_ENV_H

#include "types.h"

extern s_env g_c3_env;

/* Stack allocation compatible functions, call env_clean after use. */
void    env_clean (s_env *env);
s_env * env_init (s_env *env, int argc, s8 **argv);

/* Operators. */
bool          env_eval_array (s_env *env, const s_array *array,
                              s_array *dest);
bool          env_eval_array_tag (s_env *env, const s_array *array,
                                  s_tag *dest);
bool          env_eval_call (s_env *env, const s_call *call,
                             s_tag *dest);
bool          env_eval_call_arguments (s_env *env, const s_list *args,
                                       s_list **dest);
bool          env_eval_call_cfn (s_env *env, const s_call *call,
                                 s_tag *dest);
bool          env_eval_call_fn (s_env *env, const s_call *call,
                                s_tag *dest);
bool          env_eval_call_resolve (s_env *env, s_call *call);
bool          env_eval_equal_list (s_env *env, const s_list *a,
                                   const s_list *b, s_list **dest);
bool          env_eval_equal_struct (s_env *env, const s_struct *a,
                                     const s_struct *b, s_struct *dest);
bool          env_eval_equal_tag (s_env *env, const s_tag *a,
                                  const s_tag *b, s_tag *dest);
bool          env_eval_equal_tuple (s_env *env, const s_tuple *a,
                                    const s_tuple *b, s_tuple *dest);
bool          env_eval_fn (s_env *env, const s_fn *fn, s_tag *dest);
bool          env_eval_fn_call (s_env *env, const s_fn *fn,
                                const s_list *arguments, s_tag *dest);
bool          env_eval_ident (s_env *env, const s_ident *ident,
                              s_tag *dest);
bool          env_eval_list (s_env *env, const s_list *list,
                             s_tag *dest);
bool          env_eval_map (s_env *env, const s_map *map, s_tag *dest);
bool          env_eval_progn (s_env *env, const s_list *program,
                              s_tag *dest);
bool          env_eval_quote (s_env *env, const s_quote *quote,
                              s_tag *dest);
bool          env_eval_struct (s_env *env, const s_struct *s,
                               s_tag *dest);
bool          env_eval_tag (s_env *env, const s_tag *tag, s_tag *dest);
bool          env_eval_tuple (s_env *env, const s_tuple *tuple,
                              s_tag *dest);
bool          env_eval_void (s_env *env, const void *_, s_tag *dest);
s_list **     env_get_struct_type_spec (s_env *env, const s_sym *module,
                                        s_list **dest);
bool          env_module_load (s_env *env, const s_sym *module,
                               s_facts *facts);
bool          env_module_maybe_reload (s_env *env, const s_sym *module,
                                       s_facts *facts);
s8            env_operator_arity (s_env *env, const s_ident *op);
bool          env_operator_find (s_env *env, const s_ident *op);
s_ident *     env_operator_ident (s_env *env, const s_ident *op,
                                  s_ident *dest);
bool          env_operator_is_right_associative (s_env *env,
                                                 const s_ident *op);
s8            env_operator_precedence (s_env *env, const s_ident *op);
s_ident *     env_operator_resolve (s_env *env, const s_ident *op,
                                    u8 arity, s_ident *dest);
const s_sym * env_operator_symbol (s_env *env, const s_ident *op);
bool          env_struct_type_exists (s_env *env, const s_sym *module);
bool          env_tag_ident_is_bound (const s_env *env,
                                      const s_tag *tag, s_facts *facts);

/* Control structures. */
void env_error_f (s_env *env, const char *fmt, ...);
void env_error_tag (s_env *env, const s_tag *tag);
void env_longjmp (s_env *env, jmp_buf *jmp_buf);

#endif /* LIBC3_ENV_H */
