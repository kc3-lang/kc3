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
#ifndef LIBKC3_KC3_MAIN_H
#define LIBKC3_KC3_MAIN_H

#include "types.h"

#define KC3_EXT ".kc3"

extern const s_str g_kc3_base_binary;
extern const s_str g_kc3_base_octal;
extern const s_str g_kc3_base_decimal;
extern const s_str g_kc3_base_hexadecimal;
extern const s_str g_kc3_bases_hexadecimal[2];
extern const s_str g_kc3_base64;
extern const s_str g_kc3_base64url;
extern const char *g_kc3_license;
extern sw          g_kc3_exit_code;

/* Stack-allocation compatible functions, call kc3_clean after use. */
s_env * kc3_init (s_env *env, int *argc, char ***argv);
void    kc3_clean (s_env *env);

/* Observers. */
p_list * kc3_args (s_list **dest);
sw       kc3_dump (const s_str *path);
s_buf ** kc3_err_buf (s_buf **dest);
s_tag *  kc3_fact_from_ptr (s_tag *tag, u_ptr_w *ptr);
s_tag *  kc3_fact_object (s_fact *fact, s_tag *dest);
s_tag *  kc3_fact_predicate (s_fact *fact, s_tag *dest);
s_tag *  kc3_fact_subject (s_fact *fact, s_tag *dest);
s_tag *  kc3_facts_first_with_tags (s_facts *facts,
                                    s_tag *subject,
                                    s_tag *predicate,
                                    s_tag *object,
                                    p_callable *callback,
                                    s_tag *dest);
uw *     kc3_facts_next_id (uw *dest);
s_tag *  kc3_getenv (const s_str *name, s_tag *dest);
sw       kc3_getpid (void);
sw       kc3_getppid (void);
s_buf ** kc3_in_buf (s_buf **dest);
void     kc3_license (void);
s_tag *  kc3_list_length (const s_list **list, s_tag *dest);
p_sym *  kc3_module (const s_sym **dest);
uw *     kc3_offsetof (const s_sym * const *module,
                       const s_sym * const *field, uw *dest);
s_buf ** kc3_out_buf (s_buf **dest);
sw       kc3_puts (const s_tag *tag);
p_list * kc3_stacktrace (s_list **dest);
s_str *  kc3_str (const s_tag *tag, s_str *dest);
s32      kc3_tag_type (const s_tag *tag);
s_time * kc3_uptime (s_time *dest);

/* Operators. */
s_pointer *  kc3_address_of (s_ident *ident, s_pointer *dest);
s_tag *      kc3_access (s_tag *tag, s_list **addr,
                         s_tag *dest);
s_tag *      kc3_and (s_tag *a, s_tag *b, s_tag *dest);
s_tag *      kc3_buf_parse_tag (s_buf *buf, s_tag *dest);
s_tag *      kc3_def (const s_call *call, s_tag *dest);
s_tag *      kc3_defmodule (p_sym const *name,
                            const s_do_block *do_block, s_tag *dest);
s_tag *      kc3_defoperator (s_tag *op_tag, s_tag *dest);
s_tag *      kc3_defspecial_operator (s_tag *tag, s_tag *dest);
s_tag *      kc3_defstruct (s_list **spec, s_tag *dest);
void **      kc3_dlopen (const s_str *path, void **dest);
s_facts **   kc3_env_db (s_facts **dest);
sw           kc3_errno (void);
void         kc3_exit (s_tag *code);
bool *       kc3_facts_add_tags (s_facts *facts, s_tag *subject,
                                 s_tag *predicate,
                                 s_tag *object,
                                 bool *dest);
s_tag *      kc3_facts_collect_with (s_facts *facts, s_list **spec,
                                     p_callable *callback, s_tag *dest);
s_tag *      kc3_facts_collect_with_tags (s_facts *facts,
                                          s_tag *subject,
                                          s_tag *predicate,
                                          s_tag *object,
                                          p_callable *callback,
                                          s_tag *dest);
s_tag *      kc3_facts_first_with (s_facts *facts, s_list **spec,
                                   p_callable *callback, s_tag *dest);
s_tag *      kc3_facts_first_with_tags (s_facts *facts, s_tag *subject,
                                        s_tag *predicate, s_tag *object,
                                        p_callable *callback,
                                        s_tag *dest);
bool *       kc3_facts_remove_tags (s_facts *facts,
                                    s_tag *subject,
                                    s_tag *predicate,
                                    s_tag *object,
                                    bool *dest);
bool *       kc3_facts_replace_tags (s_facts *facts,
                                     s_tag *subject,
                                     s_tag *predicate,
                                     s_tag *object,
                                     bool *dest);
s_tag *      kc3_facts_with (s_facts *facts, s_list **spec,
                             p_callable *callback, s_tag *dest);
s_tag *      kc3_facts_with_macro (s_tag *facts_tag, s_tag *spec_tag,
                                   s_tag *do_block_tag, s_tag *dest);
s_tag *      kc3_facts_with_tags (s_facts *facts, s_tag *subject,
                                  s_tag *predicate, s_tag *object,
                                  p_callable *callback, s_tag *dest);
s_tag *      kc3_facts_with_transaction (s_tag *facts_arg,
                                         s_tag *tag_arg, s_tag *dest);
s_tag *      kc3_identity (s_tag *tag, s_tag *dest);
s_tag *      kc3_integer_reduce (s_tag *tag, s_tag *dest);
bool         kc3_killpg (sw process_group, const s_sym * const *signal);
s_tag *      kc3_let (s_tag *vars, s_tag *tag, s_tag *dest);
s_array *    kc3_list_to_array (s_list **list,
                                const s_sym * const *array_type,
                                s_array *dest);
bool         kc3_load (const s_str *path);
void         kc3_marshall_delete (p_marshall *m);
p_marshall * kc3_marshall_new (p_marshall *m);
void         kc3_marshall_read_delete (p_marshall_read *mr);
p_marshall_read * kc3_marshall_read_new (p_marshall_read *mr);
p_marshall_read * kc3_marshall_read_new_str (p_marshall_read *mr,
                                             const s_str *input);
bool         kc3_marshall_read_tag (p_marshall_read *mr,
                                    s_tag *tag);
s_tag *      kc3_marshall_read_tag_from_str (const s_str *input,
                                             s_tag *dest);
bool         kc3_marshall_tag (p_marshall *m, const s_tag *tag);
s_str *      kc3_marshall_tag_to_str (const s_tag *tag, s_str *dest);
s_str *      kc3_marshall_to_str (p_marshall *m, s_str *dest);
bool         kc3_maybe_reload (const s_str *path);
s_tag *      kc3_or (s_tag *a, s_tag *b, s_tag *dest);
s_tag *      kc3_parse_tag (s_tag *tag, const s_str *src);
bool         kc3_require (const s_sym * const *module);
s_str *      kc3_strerror (sw err_no, s_str *dest);
s_tag *      kc3_struct_put (s_tag *s, const s_sym * const *key,
                             s_tag *value, s_tag *dest);
s_str *      kc3_system (const s_list * const *list, s_str *dest);
s_tag *      kc3_thread_delete (u_ptr_w *thread, s_tag *dest);
u_ptr_w *    kc3_thread_new (u_ptr_w *dest, p_callable *start);
void *       kc3_thread_start (void *arg);

/* Special operators. */
s_tag * kc3_block (s_tag *name, s_tag *do_block, s_tag *dest);
s_tag * kc3_if_then_else (s_tag *cond, s_tag *then,
                          s_tag *else_, s_tag *dest);
void    kc3_return (s_tag *value);
void    kc3_return_from (const s_sym **name, s_tag *value);
s_tag * kc3_unwind_protect (s_tag *protected, s_tag *cleanup,
                            s_tag *dest);
s_tag * kc3_while (s_tag *cond, s_tag *body, s_tag *dest);

/* debug */
void kc3_break (void);

/* miscelleanous */
#ifdef WIN32
int fork (void);
#endif

#endif /* LIBKC3_KC3_MAIN_H */
