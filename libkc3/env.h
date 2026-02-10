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
#ifndef LIBKC3_ENV_H
#define LIBKC3_ENV_H

#include "types.h"

extern const char *g_env_argv0_default;
extern const char *g_env_argv0_dir_default;

/* Global singletons. */
s_env * env_default (void);
void    env_default_set (s_env *env);
s_env * env_global (void);
bool    env_global_set (s_env *env);

/* Stack allocation compatible functions, call env_clean after use. */
void    env_clean (s_env *env);
s_env * env_init (s_env *env, int *argc, char ***argv);
s_env * env_init_copy (s_env *env, s_env *src);

/* Heap allocation functions, call env_delete after use. */
void    env_delete (s_env *env);
s_env * env_new (int *argc, char ***argv);

/* Observers. */
s_pointer *     env_address_of (s_env *env, s_tag *tag,
                                s_pointer *dest);
s_pointer *     env_address_of_call (s_env *env, s_call *call,
                                     s_pointer *dest);
s_list **       env_args (s_env *env, s_list **dest);
bool            env_cleaning (bool enable);
sw              env_dump (const s_env *env, const s_str *path);
sw              env_dump_restore (s_env *env, const s_str *path);
bool            env_dump_restore_path_resolve (s_env *env);
s_tag *         env_frames_get (s_env *env, const s_sym *name);
s_tag *         env_ident_get_address (s_env *env, const s_ident *ident);
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
s_tag *        env_and (s_env *env, s_tag *a, s_tag *b, s_tag *dest);
bool           env_def (s_env *env, const s_ident *ident, s_tag *value);
const s_sym *  env_def_clean (s_env *env, const s_sym *module,
                              s_tag *tag_clean);
s_tag *        env_defcounter (s_env *env, s_ident *name, s_tag *value,
                               s_tag *dest);
s_tag *        env_defmodule (s_env *env, const s_sym * const *name,
                              const s_do_block *do_block, s_tag *dest);
bool           env_defoperator (s_env *env, s_tag *tag_op);
s_tag *        env_defspecial_operator (s_env *env, s_tag *tag,
                                        s_tag *dest);
const s_sym *  env_defstruct (s_env *env, s_list *spec);
void **        env_dlopen (s_env *env, const s_str *path, void **dest);
void           env_error_handler_pop (s_env *env,
                                      s_error_handler *eh);
void           env_error_handler_push (s_env *env,
                                       s_error_handler *eh);
s_fact_w *     env_fact_w_eval (s_env *env, s_fact_w *fact,
	         	       s_fact_w *dest);
s_tag *        env_facts_collect_with (s_env *env, s_facts *facts,
                                       s_list **spec,
                                       s_callable *callback,
                                       s_tag *dest);
s_tag *        env_facts_collect_with_tags (s_env *env, s_facts *facts,
                                            s_tag *subject,
                                            s_tag *predicate,
                                            s_tag *object,
                                            s_callable *callback,
                                            s_tag *dest);
s_tag *        env_facts_first_with (s_env *env, s_facts *facts,
                                     s_list **spec,
                                     s_callable *callback,
                                     s_tag *dest);
s_tag *        env_facts_first_with_tags (s_env *env, s_facts *facts,
                                          s_tag *subject,
                                          s_tag *predicate,
                                          s_tag *object,
                                          s_callable *callback,
                                          s_tag *dest);
s_tag *        env_facts_with (s_env *env, s_facts *facts,
                               s_list **spec, s_callable *callback,
                               s_tag *dest);
s_tag *        env_facts_with_macro (s_env *env, s_tag *facts_tag,
                                     s_tag *spec_tag,
                                     s_tag *do_block_tag,
                                     s_tag *dest);
s_tag *        env_facts_with_tags (s_env *env, s_facts *facts,
                                    s_tag *subject, s_tag *predicate,
                                    s_tag *object, s_callable *callback,
                                    s_tag *dest);
s_tag *        env_facts_with_transaction (s_env *env, s_tag *facts_arg,
                                           s_tag *tag_arg, s_tag *dest);
s_frame *      env_frame_new_capture (s_env *env, s_fn *fn);
s_tag *        env_ident_get (s_env *env, const s_ident *ident,
                              s_tag *dest);
bool *         env_ident_is_special_operator (s_env *env,
                                              const s_ident *ident,
                                              bool *dest);
s_tag *        env_if_then_else (s_env *env, s_tag *cond, s_tag *then,
                                 s_tag *else_, s_tag *dest);
s_tag *        env_kc3_def (s_env *env, const s_call *call,
                            s_tag *dest);
s_tag *        env_kc3_defcounter (s_env *env, s_call *call,
                                   s_tag *dest);
s_tag *        env_let (s_env *env, s_tag *vars, s_tag *tag,
                        s_tag *dest);
bool           env_load (s_env *env, const s_str *path);
sw             env_load_kc3c (s_env *env, const s_str *path);
void           env_loop_context_pop (s_env *env, s_loop_context *lc);
void           env_loop_context_push (s_env *env, s_loop_context *lc);
bool           env_maybe_reload (s_env *env, const s_str *path);
bool *         env_module_is_loading (s_env *env, const s_sym *module,
                                      bool *dest);
bool           env_module_is_loading_set (s_env *env,
                                          const s_sym *module,
                                          bool value);
bool           env_module_load (s_env *env, const s_sym *module);
bool           env_module_maybe_reload (s_env *env,
                                        const s_sym *module);
s_tag *        env_or (s_env *env, s_tag *a, s_tag *b, s_tag *dest);
s_struct_type **
               env_pstruct_type_find (s_env *env, const s_sym *module,
                                      p_struct_type *dest);
s8             env_special_operator_arity (s_env *env,
                                           const s_ident *ident);
bool *         env_struct_type_exists (s_env *env, const s_sym *module,
                                       bool *dest);
p_callable     env_struct_type_get_clean (s_env *env,
                                          const s_sym *module);
s_list **      env_struct_type_get_spec (s_env *env,
                                         const s_sym *module,
                                         s_list **dest);
bool *         env_struct_type_has_spec (s_env *env,
                                         const s_sym *module,
                                         bool *dest);
bool           env_tag_ident_is_bound (s_env *env,
                                       const s_tag *tag);
s_tag *        env_unwind_protect (s_env *env, s_tag *protected_,
                                   s_tag *cleanup,
                                   s_tag *dest);
void           env_unwind_protect_pop (s_env *env,
                                       s_unwind_protect *up);
void           env_unwind_protect_push (s_env *env,
                                        s_unwind_protect *up);
s_time *       env_uptime (s_env *env, s_time *dest);
s_tag *        env_while (s_env *env, s_tag *cond, s_tag *body,
                          s_tag *dest);

/* Modifiers. */
bool env_call_get (s_env *env, s_call *call);
bool env_module_ensure_loaded (s_env *env, const s_sym *module);

/* Control structures. */
void env_error_f (s_env *env, const char *fmt, ...);
void env_error_tag (s_env *env, s_tag *tag);
void env_longjmp (s_env *env, jmp_buf *jmp_buf);

#endif /* LIBKC3_ENV_H */
