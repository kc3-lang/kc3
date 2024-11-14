/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_ENV_H
#define LIBKC3_ENV_H

#include "types.h"

extern s_env g_kc3_env;

/* Stack allocation compatible functions, call env_clean after use. */
void    env_clean (s_env *env);
s_env * env_init (s_env *env, int *argc, char ***argv);

/* Observers. */
s_list **       env_args (s_env *env, s_list **dest);
s_tag *         env_frames_get (s_env *env, const s_sym *name);
s_ident *       env_ident_resolve_module (s_env *env,
                                          const s_ident *ident,
                                          s_ident *dest);
const s_sym **  env_module (s_env *env, const s_sym **dest);
bool *          env_module_has_ident (s_env *env, const s_sym *module,
                                      const s_ident *ident, bool *dest);
bool *          env_module_has_symbol (s_env *env, const s_sym *module,
                                       const s_sym *sym, bool *dest);
const s_time ** env_module_load_time (s_env *env, const s_sym *module,
                                      const s_time **dest);
s_list **       env_module_search_modules (s_env *env,
                                           const s_sym * const *module,
                                           s_list **dest);
s_list **       env_search_modules (s_env *env, s_list **dest);
s_list **       env_stacktrace (s_env *env, s_list **dest);
bool            env_sym_search_modules (s_env *env,
                                        const s_sym *sym,
                                        const s_sym **dest);

/* Operators. */
bool *         env_and (s_env *env, s_tag *a, s_tag *b,
                        bool *dest);
bool           env_def (s_env *env, const s_ident *ident, s_tag *value);
const s_sym *  env_def_clean (s_env *env, const s_sym *module,
                              const s_tag *tag_clean);
s_tag *        env_defmodule (s_env *env, const s_sym * const *name,
                              const s_block *block, s_tag *dest);
s_tag *        env_defoperator (s_env *env, const s_sym * const *name,
                                const s_sym * const *sym,
                                s_tag *symbol_value,
                                u8 op_precedence,
                                const s_sym * const *op_assoc,
                                s_tag *dest);
const s_sym *  env_defstruct (s_env *env, const s_list *spec);
s_fact_w *     env_fact_w_eval (s_env *env, s_fact_w *fact,
	         	       s_fact_w *dest);
s_tag *        env_facts_collect_with (s_env *env, s_facts *facts,
                                       s_list **spec, s_fn *callback,
                                       s_tag *dest);
s_tag *        env_facts_collect_with_tags (s_env *env, s_facts *facts,
                                            s_tag *subject,
                                            s_tag *predicate,
                                            s_tag *object,
                                            s_fn *callback,
                                            s_tag *dest);
s_tag *        env_facts_first_with (s_env *env, s_facts *facts,
                                     s_list **spec, s_fn *callback,
                                     s_tag *dest);
s_tag *        env_facts_first_with_tags (s_env *env, s_facts *facts,
                                          s_tag *subject,
                                          s_tag *predicate,
                                          s_tag *object,
                                          s_fn *callback, s_tag *dest);
s_tag *        env_facts_with (s_env *env, s_facts *facts,
                               s_list **spec, s_fn *callback,
                               s_tag *dest);
s_tag *        env_facts_with_macro (s_env *env, s_tag *facts_tag,
                                     s_tag *spec_tag, s_tag *block_tag,
                                     s_tag *dest);
s_tag *        env_facts_with_tags (s_env *env, s_facts *facts,
                                    s_tag *subject, s_tag *predicate,
                                    s_tag *object, s_fn *callback,
                                    s_tag *dest);
s_tag *        env_ident_get (s_env *env, const s_ident *ident,
                              s_tag *dest);
bool *         env_ident_is_special_operator (s_env *env,
                                              const s_ident *ident,
                                              bool *dest);
s_tag *        env_kc3_def (s_env *env, const s_call *call,
                            s_tag *dest);
s_tag *        env_let (s_env *env, s_tag *vars, s_tag *tag,
                        s_tag *dest);
bool           env_load (s_env *env, const s_str *path);
bool *         env_module_is_loading (s_env *env, const s_sym *module,
                                      bool *dest);
bool           env_module_is_loading_set (s_env *env,
                                          const s_sym *module,
                                          bool value);
bool           env_module_load (s_env *env, const s_sym *module);
bool           env_module_maybe_reload (s_env *env,
                                        const s_sym *module);
s8             env_operator_arity (s_env *env, const s_ident *op);
bool *         env_operator_find (s_env *env, const s_ident *op,
                                  bool *dest);
s_tag *        env_operator_find_by_sym (s_env *env,
                                         const s_sym *sym,
                                         s_tag *dest);
s_ident *      env_operator_ident (s_env *env, const s_ident *op,
                                   s_ident *dest);
bool *         env_operator_is_right_associative (s_env *env,
                                                  const s_ident *op,
                                                  bool *dest);
sw *           env_operator_precedence (s_env *env, const s_ident *op,
                                        sw *dest);
s_ident *      env_operator_resolve (s_env *env, const s_ident *op,
                                     u8 arity, s_ident *dest);
const s_sym ** env_operator_symbol (s_env *env, const s_ident *op,
                                    const s_sym **dest);
bool *         env_or (s_env *env, s_tag *a, s_tag *b,
                       bool *dest);
u8             env_special_operator_arity (s_env *env,
                                           const s_ident *ident);
bool *         env_struct_type_exists (s_env *env, const s_sym *module,
                                       bool *dest);
const s_struct_type **
               env_struct_type_find (s_env *env, const s_sym *module,
                                     const s_struct_type **dest);
f_clean        env_struct_type_get_clean (s_env *env,
                                          const s_sym *module);
s_list **      env_struct_type_get_spec (s_env *env,
                                         const s_sym *module,
                                         s_list **dest);
bool *         env_struct_type_has_spec (s_env *env,
                                         const s_sym *module,
                                         bool *dest);
bool           env_tag_ident_is_bound (s_env *env,
                                       const s_tag *tag);
s_tag *        env_while (s_env *env, s_tag *cond, s_tag *body,
                          s_tag *dest);

/* Evaluator. */
bool env_eval_array (s_env *env, const s_array *array,
                     s_array *dest);
bool env_eval_array_tag (s_env *env, const s_array *array,
                         s_tag *dest);
bool env_eval_block (s_env *env, const s_block *block,
                     s_tag *dest);
bool env_eval_call (s_env *env, s_call *call,
                    s_tag *dest);
bool env_eval_call_arguments (s_env *env, s_list *args,
                              s_list **dest);
bool env_eval_call_cfn_args (s_env *env, s_cfn *cfn, s_list *arguments,
                             s_tag *dest);
bool env_eval_call_fn (s_env *env, const s_call *call,
                       s_tag *dest);
bool env_eval_call_fn_args (s_env *env, const s_fn *fn,
                            s_list *arguments,
                            s_tag *dest);
bool env_eval_call_resolve (s_env *env, s_call *call);
bool env_eval_complex (s_env *env, s_complex *c,
                       s_tag *dest);
bool env_eval_cow (s_env *env, s_cow *cow, s_tag *dest);
bool env_eval_equal_block (s_env *env, bool macro,
                           s_block *a, s_block *b,
                           s_block *dest);
bool env_eval_equal_list (s_env *env, bool macro,
                          s_list *a, s_list *b,
                          s_list **dest);
bool env_eval_equal_struct (s_env *env, bool macro,
                            s_struct *a,
                            s_struct *b,
                            s_struct *dest);
bool env_eval_equal_tag (s_env *env, bool macro,
                         s_tag *a, s_tag *b,
                         s_tag *dest);
bool env_eval_equal_time (s_env *env, bool macro,
                          s_time *a, s_time *b,
                          s_time *dest);
bool env_eval_equal_tuple (s_env *env, bool macro,
                           s_tuple *a, s_tuple *b,
                           s_tuple *dest);
bool env_eval_ident (s_env *env, const s_ident *ident,
                     s_tag *dest);
bool env_eval_ident_is_bound (s_env *env,
                              const s_ident *ident);
bool env_eval_integer (s_env *env, const s_integer *integer,
                       s_integer *dest);
bool env_eval_list (s_env *env, s_list *list, s_tag *dest);
bool env_eval_map (s_env *env, s_map *map,
                   s_tag *dest);
bool env_eval_progn (s_env *env, s_list *program,
                     s_tag *dest);
bool env_eval_quote (s_env *env, s_quote *quote,
                     s_tag *dest);
bool env_eval_quote_array (s_env *env, s_array *array,
                           s_tag *dest);
bool env_eval_quote_block (s_env *env, s_block *block,
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
bool env_eval_struct (s_env *env, const s_struct *s,
                      s_struct *dest);
bool env_eval_struct_tag (s_env *env, const s_struct *s,
                          s_tag *dest);
bool env_eval_tag (s_env *env, s_tag *tag, s_tag *dest);
bool env_eval_time (s_env *env, const s_time *time,
                    s_tag *dest);
bool env_eval_tuple (s_env *env, const s_tuple *tuple,
                     s_tag *dest);
bool env_eval_var (s_env *env, const s_var *var, s_tag *dest);

/* Modifiers. */
bool env_call_get (s_env *env, s_call *call);
bool env_eval_callable (s_env *env, s_callable *callable,
                        s_tag *dest);
bool env_module_ensure_loaded (s_env *env, const s_sym *module);

/* Control structures. */
void env_error_f (s_env *env, const char *fmt, ...);
void env_error_tag (s_env *env, s_tag *tag);
void env_longjmp (s_env *env, jmp_buf *jmp_buf);

#endif /* LIBKC3_ENV_H */
