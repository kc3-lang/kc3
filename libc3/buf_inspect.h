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
/**
 * @file buf_inspect.h
 * @brief Inspect data into UTF-8.
 *
 * Structure to inspect data into UTF-8.
 */
#ifndef LIBC3_BUF_INSPECT_H
#define LIBC3_BUF_INSPECT_H

#include "types.h"

#define BUF_INSPECT_S_BASE_PROTOTYPES(bits, base)                      \
  sw buf_inspect_s ## bits ## _ ## base (s_buf *buf,                   \
                                         const s ## bits *s);          \
  sw buf_inspect_s ## bits ## _ ## base ## _size (const s ## bits *s)

#define BUF_INSPECT_S_PROTOTYPES(bits)                                 \
  sw buf_inspect_s ## bits (s_buf *buf, const s ## bits *s);           \
  sw buf_inspect_s ## bits ## _size (const s ## bits *s);              \
  BUF_INSPECT_S_BASE_PROTOTYPES(bits, binary);                         \
  BUF_INSPECT_S_BASE_PROTOTYPES(bits, hexadecimal);                    \
  BUF_INSPECT_S_BASE_PROTOTYPES(bits, octal)

#define BUF_INSPECT_U_BASE_PROTOTYPES(bits, base)                      \
  sw buf_inspect_u ## bits ## _ ## base (s_buf *buf,                   \
                                         const u ## bits *s);          \
  sw buf_inspect_u ## bits ## _ ## base ## _size (const u ## bits *s)

#define BUF_INSPECT_U_PROTOTYPES(bits)                                 \
  sw buf_inspect_u ## bits (s_buf *buf, const u ## bits *u);           \
  sw buf_inspect_u ## bits ## _size (const u ## bits *u);              \
  sw buf_inspect_u ## bits ## _base (s_buf *buf, const s_str *base,    \
                                     const u ## bits *u);              \
  sw buf_inspect_u ## bits ## _base_size (const s_str *base,           \
                                          const u ## bits *u);         \
  BUF_INSPECT_U_BASE_PROTOTYPES(bits, binary);                         \
  BUF_INSPECT_U_BASE_PROTOTYPES(bits, hexadecimal);                    \
  BUF_INSPECT_U_BASE_PROTOTYPES(bits, octal)

sw buf_inspect_array (s_buf *buf, const s_array *a);
sw buf_inspect_array_size (const s_array *a);
sw buf_inspect_bool (s_buf *buf, const bool *b);
sw buf_inspect_bool_size (const bool *b);
sw buf_inspect_call (s_buf *buf, const s_call *call);
sw buf_inspect_call_args (s_buf *buf, const s_list *args);
sw buf_inspect_call_brackets (s_buf *buf, const s_call *call);
sw buf_inspect_call_op (s_buf *buf, const s_call *call,
                        s8 op_precedence);
sw buf_inspect_call_op_size (const s_call *call, s8 op_precedence);
sw buf_inspect_call_op_unary (s_buf *buf, const s_call *call);
sw buf_inspect_call_op_unary_size (const s_call *call);
sw buf_inspect_call_paren (s_buf *buf, const s_call *call);
sw buf_inspect_call_paren_size (const s_call *call);
sw buf_inspect_call_size (const s_call *call);
sw buf_inspect_cast (s_buf *buf, const s_call *call);
sw buf_inspect_cast_size (const s_call *call);
sw buf_inspect_cfn (s_buf *buf, const s_cfn *cfn);
sw buf_inspect_cfn_size (const s_cfn *cfn);
sw buf_inspect_character (s_buf *buf, const character *c);
sw buf_inspect_character_size (const character *c);
sw buf_inspect_error_handler (s_buf *buf,
                              const s_error_handler *error_handler);
sw buf_inspect_f32 (s_buf *buf, const f32 *x);
sw buf_inspect_f32_size (const f32 *x);
sw buf_inspect_f64 (s_buf *buf, const f64 *x);
sw buf_inspect_f64_size (const f64 *x);
sw buf_inspect_fact (s_buf *buf, const s_fact *fact);
sw buf_inspect_fact_size (const s_fact *fact);
sw buf_inspect_facts_spec (s_buf *buf, p_facts_spec spec);
sw buf_inspect_fn (s_buf *buf, const s_fn *fn);
sw buf_inspect_fn_algo (s_buf *buf, const s_list *algo);
sw buf_inspect_fn_algo_size (const s_list *algo);
sw buf_inspect_fn_clause (s_buf *buf, const s_fn_clause *clause);
sw buf_inspect_fn_clause_size (const s_fn_clause *clause);
sw buf_inspect_fn_pattern (s_buf *buf, const s_list *pattern);
sw buf_inspect_fn_pattern_size (const s_list *pattern);
sw buf_inspect_fn_size (const s_fn *fn);
sw buf_inspect_ident (s_buf *buf, const s_ident *ident);
sw buf_inspect_ident_reserved (s_buf *buf, const s_ident *ident);
sw buf_inspect_ident_reserved_size (const s_ident *ident);
sw buf_inspect_ident_size (const s_ident *ident);
sw buf_inspect_integer (s_buf *buf, const s_integer *x);
sw buf_inspect_integer_size (const s_integer *x);
sw buf_inspect_list (s_buf *buf, const s_list * const *list);
sw buf_inspect_list_paren (s_buf *buf, const s_list * const *list);
sw buf_inspect_list_size (const s_list * const *list);
sw buf_inspect_list_tag (s_buf *buf, const s_tag *tag);
sw buf_inspect_list_tag_size (const s_tag *tag);
sw buf_inspect_map (s_buf *buf, const s_map *map);
sw buf_inspect_map_size (const s_map *map);
sw buf_inspect_paren_sym (s_buf *buf, const s_sym *sym);
sw buf_inspect_paren_sym_size (const s_sym *sym);
sw buf_inspect_ptag (s_buf *buf, const p_tag *ptag);
sw buf_inspect_ptag_size (const p_tag *ptag);
sw buf_inspect_ptr (s_buf *buf, const u_ptr_w *ptr);
sw buf_inspect_ptr_free (s_buf *buf, const u_ptr_w *ptr_free);
sw buf_inspect_ptr_free_size (const u_ptr_w *ptr_free);
sw buf_inspect_ptr_size (const u_ptr_w *ptr);
sw buf_inspect_quote (s_buf *buf, const s_quote *quote);
sw buf_inspect_quote_size (const s_quote *quote);
BUF_INSPECT_S_PROTOTYPES(8);
BUF_INSPECT_S_PROTOTYPES(16);
BUF_INSPECT_S_PROTOTYPES(32);
BUF_INSPECT_S_PROTOTYPES(64);
BUF_INSPECT_S_PROTOTYPES(w);
sw buf_inspect_str (s_buf *buf, const s_str *str);
sw buf_inspect_str_byte (s_buf *buf, const u8 *byte);
sw buf_inspect_str_byte_size (const u8 *byte);
sw buf_inspect_str_character (s_buf *buf, const character *c);
sw buf_inspect_str_character_size (const character *c);
sw buf_inspect_str_reserved (s_buf *buf, const s_str *str);
sw buf_inspect_str_reserved_size (const s_str *str);
sw buf_inspect_str_size (const s_str *str);
sw buf_inspect_struct (s_buf *buf, const s_struct *s);
sw buf_inspect_struct_size (const s_struct *s);
sw buf_inspect_sym (s_buf *buf, const s_sym * const *sym);
sw buf_inspect_sym_reserved (s_buf *buf, const s_sym *sym);
sw buf_inspect_sym_reserved_size (const s_sym *sym);
sw buf_inspect_sym_size (const s_sym * const *sym);
sw buf_inspect_tag (s_buf *buf, const s_tag *tag);
sw buf_inspect_tag_size (const s_tag *tag);
sw buf_inspect_tag_type (s_buf *buf, e_tag_type type);
sw buf_inspect_tag_type_size (e_tag_type type);
sw buf_inspect_tuple (s_buf *buf, const s_tuple *tuple);
sw buf_inspect_tuple_size (const s_tuple *tuple);
BUF_INSPECT_U_PROTOTYPES(8);
BUF_INSPECT_U_PROTOTYPES(16);
BUF_INSPECT_U_PROTOTYPES(32);
BUF_INSPECT_U_PROTOTYPES(64);
BUF_INSPECT_U_PROTOTYPES(w);
sw buf_inspect_var (s_buf *buf, const s_tag *var);
sw buf_inspect_var_size (const s_tag *var);
sw buf_inspect_void (s_buf *buf, const void *_);
sw buf_inspect_void_size (const void *_);

#endif /* LIBC3_BUF_INSPECT_H */
