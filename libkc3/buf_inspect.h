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
/**
 * @file buf_inspect.h
 * @brief Inspect data into UTF-8.
 *
 * Structure to inspect data into UTF-8.
 */
#ifndef LIBKC3_BUF_INSPECT_H
#define LIBKC3_BUF_INSPECT_H

#include "types.h"
#include "buf_inspect_s8.h"
#include "buf_inspect_s16.h"
#include "buf_inspect_s32.h"
#include "buf_inspect_s64.h"
#include "buf_inspect_sw.h"
#include "buf_inspect_u8.h"
#include "buf_inspect_u16.h"
#include "buf_inspect_u32.h"
#include "buf_inspect_u64.h"
#include "buf_inspect_uw.h"

extern const s_sym *g_buf_inspect_type;

#define BUF_INSPECT_S_BASE_PROTOTYPES(bits, base)                      \
  sw buf_inspect_s ## bits ## _ ## base (s_buf *buf,                   \
                                         const s ## bits *s);          \
  sw buf_inspect_s ## bits ## _ ## base ## _size (s_pretty *pretty,    \
                                                  const s ## bits *s)

#define BUF_INSPECT_S_PROTOTYPES(bits)                                 \
  sw buf_inspect_s ## bits (s_buf *buf, const s ## bits *s);           \
  sw buf_inspect_s ## bits ## _size (s_pretty *pretty,                 \
                                     const s ## bits *s);              \
  BUF_INSPECT_S_BASE_PROTOTYPES(bits, binary);                         \
  BUF_INSPECT_S_BASE_PROTOTYPES(bits, hexadecimal);                    \
  BUF_INSPECT_S_BASE_PROTOTYPES(bits, octal)

#define BUF_INSPECT_U_BASE_PROTOTYPES(bits, base)                      \
  sw buf_inspect_u ## bits ## _ ## base (s_buf *buf,                   \
                                         const u ## bits *s);          \
  sw buf_inspect_u ## bits ## _ ## base ## _digits (const u##bits *s); \
  sw buf_inspect_u ## bits ## _ ## base ## _size (s_pretty *pretty,    \
                                                  const u ## bits *s)

#define BUF_INSPECT_U_PROTOTYPES(bits)                                 \
  sw buf_inspect_u ## bits (s_buf *buf, const u ## bits *u);           \
  sw buf_inspect_u ## bits ## _digits (const u ## bits *u);            \
  sw buf_inspect_u ## bits ## _size (s_pretty *pretty,                 \
                                     const u ## bits *u);              \
  sw buf_inspect_u ## bits ## _base (s_buf *buf, const s_str *base,    \
                                     const u ## bits *u);              \
  sw buf_inspect_u ## bits ## _base_digits (const s_str *base,         \
                                            const u ## bits *u);       \
  sw buf_inspect_u ## bits ## _base_size (s_pretty *pretty,            \
                                          const s_str *base,           \
                                          const u ## bits *u);         \
  BUF_INSPECT_U_BASE_PROTOTYPES(bits, binary);                         \
  BUF_INSPECT_U_BASE_PROTOTYPES(bits, hexadecimal);                    \
  BUF_INSPECT_U_BASE_PROTOTYPES(bits, octal)

sw buf_inspect_array (s_buf *buf, const s_array *a);
sw buf_inspect_array_size (s_pretty *pretty, const s_array *a);
sw buf_inspect_block (s_buf *buf, const s_block *b);
sw buf_inspect_block_inner (s_buf *buf, const s_block *b);
sw buf_inspect_block_inner_size (s_pretty *pretty, const s_block *b);
sw buf_inspect_block_size (s_pretty *pretty, const s_block *b);
sw buf_inspect_bool (s_buf *buf, const bool *b);
sw buf_inspect_bool_size (s_pretty *pretty, const bool *b);
sw buf_inspect_call (s_buf *buf, const s_call *call);
sw buf_inspect_call_access (s_buf *buf, const s_call *call);
sw buf_inspect_call_access_size (s_pretty *pretty, const s_call *call);
sw buf_inspect_call_args (s_buf *buf, const s_list *args);
sw buf_inspect_call_brackets (s_buf *buf, const s_call *call);
sw buf_inspect_call_if_then_else (s_buf *buf, const s_call *call);
sw buf_inspect_call_if_then_else_size (s_pretty *pretty,
                                       const s_call *call);
sw buf_inspect_call_op (s_buf *buf, const s_call *call,
                        sw op_precedence);
sw buf_inspect_call_op_size (s_pretty *pretty, const s_call *call,
                             sw op_precedence);
sw buf_inspect_call_op_unary (s_buf *buf, const s_call *call);
sw buf_inspect_call_op_unary_size (s_pretty *pretty,
                                   const s_call *call);
sw buf_inspect_call_paren (s_buf *buf, const s_call *call);
sw buf_inspect_call_paren_size (s_pretty *pretty, const s_call *call);
sw buf_inspect_call_size (s_pretty *pretty, const s_call *call);
sw buf_inspect_call_special_operator (s_buf *buf, const s_call *call);
sw buf_inspect_call_special_operator_size (s_pretty *pretty,
                                           const s_call *call);
sw buf_inspect_call_str (s_buf *buf, const s_call *call);
sw buf_inspect_call_str_size (s_pretty *pretty, const s_call *call);
sw buf_inspect_callable (s_buf *buf, const s_callable *callable);
sw buf_inspect_callable_size (s_pretty *pretty,
                              const s_callable *callable);
sw buf_inspect_cast (s_buf *buf, const s_call *call);
sw buf_inspect_cast_size (s_pretty *pretty, const s_call *call);
sw buf_inspect_cfn (s_buf *buf, const s_cfn *cfn);
sw buf_inspect_cfn_size (s_pretty *pretty, const s_cfn *cfn);
sw buf_inspect_character (s_buf *buf, const character *c);
sw buf_inspect_character_size (s_pretty *pretty, const character *c);
sw buf_inspect_complex (s_buf *buf, const s_complex *c);
sw buf_inspect_complex_size (s_pretty *pretty, const s_complex *c);
sw buf_inspect_cow (s_buf *buf, s_cow *cow);
sw buf_inspect_cow_size (s_pretty *pretty, s_cow *cow);
sw buf_inspect_error_handler (s_buf *buf,
                              const s_error_handler *error_handler);
sw buf_inspect_f32 (s_buf *buf, const f32 *x);
sw buf_inspect_f32_size (s_pretty *pretty, const f32 *x);
sw buf_inspect_f64 (s_buf *buf, const f64 *x);
sw buf_inspect_f64_size (s_pretty *pretty, const f64 *x);
sw buf_inspect_f128 (s_buf *buf, const f128 *x);
sw buf_inspect_f128_size (s_pretty *pretty, const f128 *x);
sw buf_inspect_fact (s_buf *buf, const s_fact *fact);
sw buf_inspect_fact_size (s_pretty *pretty, const s_fact *fact);
sw buf_inspect_facts_spec (s_buf *buf, p_facts_spec spec);
sw buf_inspect_fn (s_buf *buf, const s_fn *fn);
sw buf_inspect_fn_clause (s_buf *buf, const s_fn_clause *clause);
sw buf_inspect_fn_clause_size (s_pretty *pretty,
                               const s_fn_clause *clause);
sw buf_inspect_fn_pattern (s_buf *buf, const s_list *pattern);
sw buf_inspect_fn_pattern_size (s_pretty *pretty,
                                const s_list *pattern);
sw buf_inspect_fn_size (s_pretty *pretty, const s_fn *fn);
sw buf_inspect_frame (s_buf *buf, const s_frame *frame);
sw buf_inspect_ident (s_buf *buf, const s_ident *ident);
sw buf_inspect_ident_size (s_pretty *pretty, const s_ident *ident);
sw buf_inspect_ident_sym (s_buf *buf, const s_sym *sym);
sw buf_inspect_ident_sym_reserved (s_buf *buf, const s_sym *sym);
sw buf_inspect_ident_sym_reserved_size (s_pretty *pretty,
                                        const s_sym *sym);
sw buf_inspect_ident_sym_size (s_pretty *pretty, const s_sym *sym);
sw buf_inspect_integer (s_buf *buf, const s_integer *x);
sw buf_inspect_integer_size (s_pretty *pretty, const s_integer *x);
sw buf_inspect_list (s_buf *buf, const s_list * const *list);
sw buf_inspect_list_paren (s_buf *buf, const s_list * const *list);
sw buf_inspect_list_size (s_pretty *pretty, const s_list * const *list);
sw buf_inspect_list_tag (s_buf *buf, const s_tag *tag);
sw buf_inspect_list_tag_size (s_pretty *pretty, const s_tag *tag);
sw buf_inspect_map (s_buf *buf, const s_map *map);
sw buf_inspect_map_size (s_pretty *pretty, const s_map *map);
sw buf_inspect_paren_sym (s_buf *buf, const s_sym *sym);
sw buf_inspect_paren_sym_size (s_pretty *pretty, const s_sym *sym);
sw buf_inspect_pointer (s_buf *buf, const void *ptr);
sw buf_inspect_pointer_size (s_pretty *pretty, const void *ptr);
sw buf_inspect_ptag (s_buf *buf, const p_tag *ptag);
sw buf_inspect_ptag_size (s_pretty *pretty, const p_tag *ptag);
sw buf_inspect_ptr (s_buf *buf, const u_ptr_w *ptr);
sw buf_inspect_ptr_free (s_buf *buf, const u_ptr_w *ptr_free);
sw buf_inspect_ptr_free_size (s_pretty *pretty,
                              const u_ptr_w *ptr_free);
sw buf_inspect_ptr_size (s_pretty *pretty, const u_ptr_w *ptr);
sw buf_inspect_quote (s_buf *buf, const s_quote *quote);
sw buf_inspect_quote_size (s_pretty *pretty, const s_quote *quote);
sw buf_inspect_ratio (s_buf *buf, const s_ratio *x);
sw buf_inspect_ratio_size (s_pretty *pretty, const s_ratio *x);
BUF_INSPECT_S_PROTOTYPES(8);
BUF_INSPECT_S_PROTOTYPES(16);
BUF_INSPECT_S_PROTOTYPES(32);
BUF_INSPECT_S_PROTOTYPES(64);
sw buf_inspect_stacktrace (s_buf *buf, const s_list *stacktrace);
sw buf_inspect_stacktrace_size (s_pretty *pretty,
                                const s_list *stacktrace);
sw buf_inspect_str (s_buf *buf, const s_str *str);
sw buf_inspect_str_byte (s_buf *buf, const u8 *byte);
sw buf_inspect_str_byte_size (s_pretty *pretty, const u8 *byte);
sw buf_inspect_str_character (s_buf *buf, const character *c);
sw buf_inspect_str_character_size (s_pretty *pretty,
                                   const character *c);
sw buf_inspect_str_eval (s_buf *buf, const s_list *list);
sw buf_inspect_str_eval_size (s_pretty *pretty, const s_list *list);
sw buf_inspect_str (s_buf *buf, const s_str *str);
sw buf_inspect_str_reserved (s_buf *buf, const s_str *str, bool quotes);
sw buf_inspect_str_reserved_size (s_pretty *pretty, const s_str *str,
                                  bool quotes);
sw buf_inspect_str_size (s_pretty *pretty, const s_str *str);
sw buf_inspect_struct (s_buf *buf, const s_struct *s);
sw buf_inspect_struct_size (s_pretty *pretty, const s_struct *s);
sw buf_inspect_struct_type (s_buf *buf, const s_struct_type *st);
sw buf_inspect_struct_type_size (s_pretty *pretty,
                                 const s_struct_type *st);
BUF_INSPECT_S_PROTOTYPES(w);
sw buf_inspect_sym (s_buf *buf, const s_sym * const *sym);
sw buf_inspect_sym_reserved (s_buf *buf, const s_sym *sym);
sw buf_inspect_sym_reserved_size (s_pretty *pretty, const s_sym *sym);
sw buf_inspect_sym_size (s_pretty *pretty, const s_sym * const *sym);
sw buf_inspect_tag (s_buf *buf, const s_tag *tag);
sw buf_inspect_tag_size (s_pretty *pretty, const s_tag *tag);
sw buf_inspect_tag_type (s_buf *buf, e_tag_type type);
sw buf_inspect_tag_type_size (s_pretty *pretty, e_tag_type type);
sw buf_inspect_time (s_buf *buf, const s_time *time);
sw buf_inspect_time_size (s_pretty *pretty, const s_time *time);
sw buf_inspect_tuple (s_buf *buf, const s_tuple *tuple);
sw buf_inspect_tuple_size (s_pretty *pretty, const s_tuple *tuple);
BUF_INSPECT_U_PROTOTYPES(8);
BUF_INSPECT_U_PROTOTYPES(16);
BUF_INSPECT_U_PROTOTYPES(32);
BUF_INSPECT_U_PROTOTYPES(64);
sw buf_inspect_unquote (s_buf *buf, const s_unquote *unquote);
sw buf_inspect_unquote_size (s_pretty *pretty,
                             const s_unquote *unquote);
BUF_INSPECT_U_PROTOTYPES(w);
sw buf_inspect_var (s_buf *buf, const s_var *var);
sw buf_inspect_var_size (s_pretty *pretty, const s_var *var);
sw buf_inspect_void (s_buf *buf, const void *v);
sw buf_inspect_void_size (s_pretty *pretty, const void *v);

#endif /* LIBKC3_BUF_INSPECT_H */
