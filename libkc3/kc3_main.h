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
#ifndef LIBKC3_KC3_MAIN_H
#define LIBKC3_KC3_MAIN_H

#include "types.h"

#define KC3_EXT ".kc3"

extern const s_str g_kc3_base_binary;
extern const s_str g_kc3_base_octal;
extern const s_str g_kc3_base_decimal;
extern const s_str g_kc3_base_hexadecimal;
extern const s_str g_kc3_bases_hexadecimal[2];
extern const s_str g_kc3_base32;
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
s_str *  kc3_hostname (s_str *dest);
s_buf ** kc3_in_buf (s_buf **dest);
void     kc3_license (void);
s_tag *  kc3_list_length (const s_list **list, s_tag *dest);
p_list * kc3_map_map_to_list (s_map *map, p_callable *callable,
                              p_list *dest);
p_sym *  kc3_module (const s_sym **dest);
uw *     kc3_offsetof (p_sym *module,
                       p_sym *field, uw *dest);
s_buf ** kc3_out_buf (s_buf **dest);
sw       kc3_puts (const s_tag *tag);
p_list * kc3_stacktrace (s_list **dest);
s_str *  kc3_str (const s_tag *tag, s_str *dest);
s32      kc3_tag_type (const s_tag *tag);
s_time * kc3_uptime (s_time *dest);
p_tuple * kc3_wait (p_tuple *dest);

/* Operators. */
s_pointer *          kc3_address_of (s_tag *tag, s_pointer *dest);
s_pointer *          kc3_tag_to_pointer (s_tag *tag, s_pointer *dest);
s_tag *              kc3_access (s_tag *tag, s_list **addr,
                                 s_tag *dest);
s_tag *              kc3_and (s_tag *a, s_tag *b, s_tag *dest);
s_tag *              kc3_buf_parse_tag (s_buf *buf, s_tag *dest);
s_tag *              kc3_def (p_call *pcall, s_tag *dest);
s_tag *              kc3_counter_decrease (s_counter **counter,
                                           s_tag *positive,
                                           s_tag *dest);
void                 kc3_counter_delete (s_counter **counter);
s_tag *              kc3_counter_get (s_counter **counter, s_tag *dest);
s_tag *              kc3_counter_increase (s_counter **counter,
                                           s_tag *positive,
                                           s_tag *dest);
s_counter **         kc3_counter_new (s_counter **dest, s_tag *value);
s_tag *              kc3_defmodule (p_sym const *name,
                                    const s_do_block *do_block,
                                    s_tag *dest);
s_tag *              kc3_defoperator (s_tag *op_tag, s_tag *dest);
s_tag *              kc3_defspecial_operator (s_tag *tag, s_tag *dest);
s_tag *              kc3_defstruct (s_list **spec, s_tag *dest);
void **              kc3_dlopen (const s_str *path, void **dest);
s_facts **           kc3_env_db (s_facts **dest);
sw                   kc3_errno (void);
void                 kc3_exit (s_tag *code);
s_facts_connection * kc3_facts_accept (s_facts *facts,
                                       p_socket server_fd,
                                       p_tls *tls);
s_facts_acceptor **  kc3_facts_acceptor_loop (s_facts *facts,
                                             p_socket server,
                                             p_tls *tls,
                                             s_facts_acceptor **dest);
s_facts *            kc3_facts_connect (s_facts *facts,
                                        const s_str *host,
                                        const s_str *service,
                                        p_tls_config *config);
bool *               kc3_facts_add_tags (s_facts *facts, s_tag *subject,
                                         s_tag *predicate,
                                         s_tag *object,
                                         bool *dest);
s_tag *              kc3_facts_collect_with (s_facts *facts,
                                             s_list **spec,
                                             p_callable *callback,
                                             s_tag *dest);
s_tag *              kc3_facts_collect_with_tags (s_facts *facts,
                                                  s_tag *subject,
                                                  s_tag *predicate,
                                                  s_tag *object,
                                                  p_callable *callback,
                                                  s_tag *dest);
s_tag *              kc3_facts_first_with (s_facts *facts,
                                           s_list **spec,
                                           p_callable *callback,
                                           s_tag *dest);
s_tag *              kc3_facts_first_with_tags (s_facts *facts,
                                                s_tag *subject,
                                                s_tag *predicate,
                                                s_tag *object,
                                                p_callable *callback,
                                                s_tag *dest);
bool *               kc3_facts_remove_tags (s_facts *facts,
                                            s_tag *subject,
                                            s_tag *predicate,
                                            s_tag *object,
                                            bool *dest);
bool *               kc3_facts_replace_tags (s_facts *facts,
                                             s_tag *subject,
                                             s_tag *predicate,
                                             s_tag *object,
                                             bool *dest);
s_facts *            kc3_facts_set_priority (s_facts *facts,
                                             u8 priority);
s_facts *            kc3_facts_set_secret (s_facts *facts,
                                           const s_str *secret);
s_facts *            kc3_facts_set_server_count (s_facts *facts,
                                                 uw server_count);
s_tag *              kc3_facts_with (s_facts *facts, s_list **spec,
                                     p_callable *callback, s_tag *dest);
s_tag *              kc3_facts_with_macro (s_tag *facts_tag,
                                           s_tag *spec_tag,
                                           s_tag *do_block_tag,
                                           s_tag *dest);
s_tag *              kc3_facts_with_tags (s_facts *facts,
                                          s_tag *subject,
                                          s_tag *predicate,
                                          s_tag *object,
                                          p_callable *callback,
                                          s_tag *dest);
s_tag *              kc3_facts_with_transaction (s_tag *facts_arg,
                                                 s_tag *tag_arg,
                                                 s_tag *dest);
s_tag *              kc3_identity (s_tag *tag, s_tag *dest);
s_tag *              kc3_integer_reduce (s_tag *tag, s_tag *dest);
bool                 kc3_killpg (sw process_group, p_sym *signal);
s_tag *              kc3_let (s_tag *vars, s_tag *tag, s_tag *dest);
s_array *            kc3_list_to_array (s_list **list,
                                        p_sym *array_type,
                                        s_array *dest);
bool                 kc3_load (const s_str *path);
void                 kc3_marshall_delete (p_marshall *m);
p_marshall *         kc3_marshall_new (p_marshall *m, uw buf_size);
void                 kc3_marshall_read_delete (p_marshall_read *mr);
p_marshall_read *    kc3_marshall_read_new (p_marshall_read *mr);
p_marshall_read *    kc3_marshall_read_new_str (p_marshall_read *mr,
                                               const s_str *input);
bool                 kc3_marshall_read_tag (p_marshall_read *mr,
                                            s_tag *tag);
s_tag *              kc3_marshall_read_tag_from_str (const s_str *input,
                                                     s_tag *dest);
bool                 kc3_marshall_tag (p_marshall *m, const s_tag *tag);
s_str *              kc3_marshall_tag_to_str (const s_tag *tag,
                                              s_str *dest);
s_str *              kc3_marshall_to_str (p_marshall *m, s_str *dest);
bool                 kc3_maybe_reload (const s_str *path);
void                 kc3_mutex_delete (s_mutex **mutex);
void                 kc3_mutex_lock (s_mutex **mutex);
s_mutex **           kc3_mutex_new (s_mutex **dest);
void                 kc3_mutex_unlock (s_mutex **mutex);
s_tag *              kc3_or (s_tag *a, s_tag *b, s_tag *dest);
s_tag *              kc3_parse_tag (s_tag *tag, const s_str *src);
void                 kc3_pledge (const s_str *promises,
                                 const s_str *execpromises);
s_tag *              kc3_require (p_sym *module, s_tag *dest);
s_str *              kc3_strerror (sw err_no, s_str *dest);
s_tag *              kc3_struct_put (s_tag *s, p_sym *key,
                                     s_tag *value, s_tag *dest);
p_tuple *            kc3_system (p_list *list, p_tuple *dest);
s_tag *              kc3_thread_delete (u_ptr_w *thread, s_tag *dest);
u_ptr_w *            kc3_thread_new (u_ptr_w *dest, p_callable *start);
void *               kc3_thread_start (void *arg);
void                 kc3_unveil (const s_str *path,
                                 const s_str *permissions);

/* Special operators. */
s_tag * kc3_block (s_tag *name, s_tag *do_block, s_tag *dest);
s_tag * kc3_if_then_else (s_tag *cond, s_tag *then,
                          s_tag *else_, s_tag *dest);
void    kc3_return (s_tag *value);
void    kc3_return_from (const s_sym **name, s_tag *value);
s_tag * kc3_unwind_protect (s_tag *protected_, s_tag *cleanup,
                            s_tag *dest);
s_tag * kc3_while (s_tag *cond, s_tag *body, s_tag *dest);

/* debug */
void kc3_break (void);

/* miscelleanous */
#ifdef WIN32
int fork (void);
#endif

#endif /* LIBKC3_KC3_MAIN_H */
