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
#ifndef LIBKC3_KC3_MAIN_H
#define LIBKC3_KC3_MAIN_H

#include "types.h"

#define KC3_EXT ".kc3"

extern const s_str g_kc3_base_binary;
extern const s_str g_kc3_base_octal;
extern const s_str g_kc3_base_decimal;
extern const s_str g_kc3_base_hexadecimal;
extern const s_str g_kc3_bases_hexadecimal[2];
extern const char *g_kc3_license;
extern sw          g_kc3_exit_code;

/* Stack-allocation compatible functions, call kc3_clean after use. */
s_env * kc3_init (s_env *env, int *argc, char ***argv);
void    kc3_clean (s_env *env);

/* Observers. */
s_tag *        kc3_fact_from_ptr (s_tag *tag, u_ptr_w *ptr);
s_tag *        kc3_fact_object (s_fact *fact, s_tag *dest);
s_tag *        kc3_fact_predicate (s_fact *fact, s_tag *dest);
s_tag *        kc3_fact_subject (s_fact *fact, s_tag *dest);
s_tag *        kc3_facts_first_with_tags (s_facts *facts,
                                          s_tag *subject,
                                          s_tag *predicate,
                                          s_tag *object,
                                          s_fn *callback, s_tag *dest);
uw *           kc3_facts_next_id (uw *dest);
s_str *        kc3_getenv (const s_str *name, s_str *dest);
void           kc3_license (void);
const s_sym ** kc3_module (const s_sym **dest);
uw *           kc3_offsetof (const s_sym * const *module,
                             const s_sym * const *field, uw *dest);
sw             kc3_puts (const s_tag *tag);
s_str *        kc3_str (const s_tag *tag, s_str *dest);

/* Operators. */
s_tag *      kc3_access (const s_tag *tag, const s_list * const *addr,
                         s_tag *dest);
bool *       kc3_and (const s_tag *a, const s_tag *b, bool *dest);
s_tag *      kc3_buf_parse_tag (s_buf *buf, s_tag *dest);
s_tag *      kc3_def (const s_call *call, s_tag *dest);
s_tag *      kc3_defmodule (const s_sym **name, const s_block *block,
                            s_tag *dest);
s_tag *      kc3_defoperator (const s_sym **name, const s_sym **sym,
                              const s_tag *symbol_value,
                              u8 operator_precedence,
                              const s_sym **operator_associativity,
                              s_tag *dest);
s_tag *      kc3_defstruct (const s_list * const *spec, s_tag *dest);
void **      kc3_dlopen (const s_str *path, void **dest);
s_facts *    kc3_env_facts (void);
sw           kc3_errno (void);
void         kc3_exit (sw code);
s_tag *      kc3_facts_collect_with_tags (s_facts *facts,
                                          s_tag *subject,
                                          s_tag *predicate,
                                          s_tag *object,
                                          s_fn *callback,
                                          s_tag *dest);
s_tag *      kc3_facts_with (s_facts *facts, s_list **spec,
                             s_fn *callback, s_tag *dest);
s_tag *      kc3_facts_with_tags (s_facts *facts, s_tag *subject,
                                  s_tag *predicate, s_tag *object,
                                  s_fn *callback, s_tag *dest);
s_tag *      kc3_identity (const s_tag *tag, s_tag *dest);
bool         kc3_load (const s_str *path);
s_tag *      kc3_operator_find_by_sym (const s_sym * const *sym,
                                       s_tag *dest);
bool *       kc3_or (const s_tag *a, const s_tag *b, bool *dest);
bool         kc3_require (const s_sym * const *module);
s_str *      kc3_strerror (sw err_no, s_str *dest);

/* Special operators. */
s_tag * kc3_if_then_else (const s_tag *cond, const s_tag *then,
                          const s_tag *else_, s_tag *dest);
s_tag * kc3_while (const s_tag *cond, const s_tag *body, s_tag *dest);

/* debug */
void kc3_break (void);

#endif /* LIBKC3_KC3_MAIN_H */
