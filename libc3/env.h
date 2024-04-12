/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
s_env * env_init (s_env *env, int argc, char **argv);

/* Observers. */
const s_tag * env_frames_get (const s_env *env, const s_sym *name);
s_ident *     env_ident_resolve_module (const s_env *env,
                                        const s_ident *ident,
                                        s_ident *dest);

/* Operators. */
s_tag *       env_def (s_env *env, const s_call *call, s_tag *dest);
s_tag *       env_defmodule (s_env *env, const s_sym **name,
                             const s_block *block, s_tag *dest);
s_tag *       env_defoperator (s_env *env, const s_sym **name,
                               const s_sym **sym,
                               const s_tag *symbol_value,
                               u8 operator_precedence,
                               const s_sym **operator_associativity,
                               s_tag *dest);
bool          env_eval_array (s_env *env, const s_array *array,
                              s_array *dest);
bool          env_eval_array_tag (s_env *env, const s_array *array,
                                  s_tag *dest);
bool          env_eval_block (s_env *env, const s_block *block,
                              s_tag *dest);
bool          env_eval_call (s_env *env, const s_call *call,
                             s_tag *dest);
bool          env_eval_call_arguments (s_env *env, const s_list *args,
                                       s_list **dest);
bool          env_eval_call_cfn (s_env *env, const s_call *call,
                                 s_tag *dest);
bool          env_eval_call_fn (s_env *env, const s_call *call,
                                s_tag *dest);
bool          env_eval_call_fn_args (s_env *env, const s_fn *fn,
                                     const s_list *arguments,
                                     s_tag *dest);
bool          env_eval_call_resolve (s_env *env, s_call *call);
bool          env_eval_complex (s_env *env, const s_complex *c,
                                s_tag *dest);
bool          env_eval_equal_block (s_env *env, bool macro,
                                    const s_block *a, const s_block *b,
                                    s_block *dest);
bool          env_eval_equal_list (s_env *env, bool macro,
                                   const s_list *a, const s_list *b,
                                   s_list **dest);
bool          env_eval_equal_struct (s_env *env, bool macro,
                                     const s_struct *a,
                                     const s_struct *b,
                                     s_struct *dest);
bool          env_eval_equal_tag (s_env *env, bool macro,
                                  const s_tag *a, const s_tag *b,
                                  s_tag *dest);
bool          env_eval_equal_tuple (s_env *env, bool macro,
                                    const s_tuple *a, const s_tuple *b,
                                    s_tuple *dest);
bool          env_eval_fn (s_env *env, const s_fn *fn, s_tag *dest);
bool          env_eval_fn_tag (s_env *env, const s_tag *tag,
                               s_tag *dest);
bool          env_eval_fn_tag_array (s_env *env, const s_array *array,
                                     s_tag *dest);
bool          env_eval_fn_tag_block (s_env *env, const s_block *block,
                                     s_tag *dest);
bool          env_eval_fn_tag_call (s_env *env, const s_call *call,
                                    s_tag *dest);
bool          env_eval_fn_tag_complex (s_env *env, const s_complex *c,
                                       s_tag *dest);
bool          env_eval_fn_tag_ident (s_env *env, const s_ident *ident,
                                     s_tag *dest);
bool          env_eval_fn_tag_list (s_env *env, const s_list *list,
                                    s_tag *dest);
bool          env_eval_fn_tag_map (s_env *env, const s_map *map,
                                   s_tag *dest);
bool          env_eval_fn_tag_quote (s_env *env, const s_quote *quote,
                                     s_tag *dest);
bool          env_eval_fn_tag_str (s_env *env, const s_str *str,
                                   s_tag *dest);
bool          env_eval_fn_tag_struct (s_env *env, const s_struct *s,
                                      s_tag *dest);
bool          env_eval_fn_tag_tag (s_env *env, const s_tag *tag,
                                   s_tag *dest);
bool          env_eval_fn_tag_tuple (s_env *env, const s_tuple *tuple,
                                     s_tag *dest);
bool          env_eval_fn_tag_unquote (s_env *env,
                                       const s_unquote *unquote,
                                       s_tag *dest);
bool          env_eval_ident (s_env *env, const s_ident *ident,
                              s_tag *dest);
bool          env_eval_ident_is_bound (s_env *env,
                                       const s_ident *ident);
bool          env_eval_list (s_env *env, const s_list *list,
                             s_tag *dest);
bool          env_eval_map (s_env *env, const s_map *map,
                            s_tag *dest);
bool          env_eval_progn (s_env *env, const s_list *program,
                              s_tag *dest);
bool          env_eval_quote (s_env *env, const s_quote *quote,
                              s_tag *dest);
bool          env_eval_quote_array (s_env *env, const s_array *array,
                                    s_tag *dest);
bool          env_eval_quote_block (s_env *env, const s_block *block,
                                    s_tag *dest);
bool          env_eval_quote_call (s_env *env, const s_call *call,
                                   s_tag *dest);
bool          env_eval_quote_complex (s_env *env, const s_complex *c,
                                      s_tag *dest);
bool          env_eval_quote_list (s_env *env, const s_list *list,
                                   s_tag *dest);
bool          env_eval_quote_map (s_env *env, const s_map *map,
                                  s_tag *dest);
bool          env_eval_quote_quote (s_env *env, const s_quote *quote,
                                    s_tag *dest);
bool          env_eval_quote_struct (s_env *env, const s_struct *s,
                                     s_tag *dest);
bool          env_eval_quote_tag (s_env *env, const s_tag *tag,
                                  s_tag *dest);
bool          env_eval_quote_tuple (s_env *env, const s_tuple *tuple,
                                    s_tag *dest);
bool          env_eval_quote_unquote (s_env *env,
                                      const s_unquote *unquote,
                                      s_tag *dest);
bool          env_eval_struct (s_env *env, const s_struct *s,
                               s_tag *dest);
bool          env_eval_tag (s_env *env, const s_tag *tag,
                            s_tag *dest);
bool          env_eval_tuple (s_env *env, const s_tuple *tuple,
                              s_tag *dest);
bool          env_eval_void (s_env *env, const void *_, s_tag *dest);
bool          env_ident_is_special_operator (s_env *env,
                                             const s_ident *ident);
bool          env_module_is_loading (s_env *env, const s_sym *module);
bool          env_module_is_loading_set (s_env *env,
                                         const s_sym *module,
                                         bool value);
bool          env_module_load (s_env *env, const s_sym *module,
                               s_facts *facts);
bool          env_module_maybe_reload (s_env *env,
                                       const s_sym *module,
                                       s_facts *facts);
s8            env_operator_arity (s_env *env, const s_ident *op);
bool          env_operator_find (s_env *env, const s_ident *op);
s_ident *     env_operator_ident (s_env *env, const s_ident *op,
                                  s_ident *dest);
bool          env_eval_integer (s_env *env, const s_integer *integer,
                                s_integer *dest);
bool          env_operator_is_right_associative (s_env *env,
                                                 const s_ident *op);
s8            env_operator_precedence (s_env *env, const s_ident *op);
s_ident *     env_operator_resolve (s_env *env, const s_ident *op,
                                    u8 arity, s_ident *dest);
const s_sym * env_operator_symbol (s_env *env, const s_ident *op);
u8            env_special_operator_arity (s_env *env,
                                          const s_ident *ident);
bool          env_struct_type_exists (s_env *env,
                                      const s_sym *module);
const s_struct_type *
              env_struct_type_find (s_env *env, const s_sym *module);
f_clean       env_struct_type_get_clean (s_env *env,
                                         const s_sym *module);
s_list **     env_struct_type_get_spec (s_env *env, const s_sym *module,
                                        s_list **dest);
bool          env_tag_ident_is_bound (const s_env *env,
                                      const s_tag *tag,
                                      s_facts *facts);

/* Control structures. */
void env_error_f (s_env *env, const char *fmt, ...);
void env_error_tag (s_env *env, const s_tag *tag);
void env_longjmp (s_env *env, jmp_buf *jmp_buf);

#endif /* LIBC3_ENV_H */
