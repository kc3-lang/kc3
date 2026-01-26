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
/**
 * @file buf_parse.h
 * @brief Byte buffer.
 *
 * Structure to manipulate byte buffers.
 */
#ifndef LIBKC3_BUF_PARSE_H
#define LIBKC3_BUF_PARSE_H

#include "buf_parse_s8.h"
#include "buf_parse_s16.h"
#include "buf_parse_s32.h"
#include "buf_parse_s64.h"
#include "buf_parse_sw.h"
#include "buf_parse_u8.h"
#include "buf_parse_u16.h"
#include "buf_parse_u32.h"
#include "buf_parse_u64.h"
#include "buf_parse_uw.h"

/**
 * # buf_parse_
 *
 * Parse UTF-8 bytes from buf and fills dest if successful.
 *
 * Returns number of bytes read in buf, possibly zero in which case
 * dest is untouched.
 */

#define BUF_PARSE_TAG_NO_BRACKETS 1

sw buf_parse_array (s_buf *buf, s_array *dest);
sw buf_parse_array_data (s_buf *buf, s_array *dest);
sw buf_parse_array_dimension_count (s_buf *buf, s_array *dest);
sw buf_parse_array_dimensions (s_buf *buf, s_array *dest);
sw buf_parse_bool (s_buf *buf, bool *dest);
sw buf_parse_call_op_rec (s_buf *buf, p_call dest, u8 min_precedence);
sw buf_parse_cfn (s_buf *buf, s_cfn *dest);
sw buf_parse_character (s_buf *buf, character *dest);
sw buf_parse_comments (s_buf *buf);
sw buf_parse_cow (s_buf *buf, s_cow *cow);
sw buf_parse_digit (s_buf *buf, const s_str *base, u8 *dest);
sw buf_parse_digit_bin (s_buf *buf, u8 *dest);
sw buf_parse_digit_hex (s_buf *buf, u8 *dest);
sw buf_parse_digit_oct (s_buf *buf, u8 *dest);
sw buf_parse_digit_dec (s_buf *buf, u8 *dest);
sw buf_parse_do_block (s_buf *buf, s_do_block *dest);
sw buf_parse_do_block_inner (s_buf *buf, f_buf_parse_end f_end,
                             s_do_block *do_block);
sw buf_parse_do_block_inner_end (s_buf *buf, bool *dest);
sw buf_parse_do_block_inner_end_short (s_buf *buf, bool *dest);
sw buf_parse_f32 (s_buf *buf, f32 *dest);
sw buf_parse_f64 (s_buf *buf, f64 *dest);
sw buf_parse_fact (s_buf *buf, s_fact *dest);
sw buf_parse_fact_w (s_buf *buf, s_fact_w *dest);
sw buf_parse_fn (s_buf *buf, s_fn *dest);
sw buf_parse_fn_clause (s_buf *buf, s_fn_clause *dest);
sw buf_parse_fn_pattern (s_buf *buf, s_list **dest);
sw buf_parse_ident (s_buf *buf, s_ident *dest);
sw buf_parse_ident_sym (s_buf *buf, const s_sym **dest);
sw buf_parse_if_then (s_buf *buf, s_tag *dest, bool *has_else);
sw buf_parse_integer (s_buf *buf, s_integer *dest);
sw buf_parse_integer_decimal (s_buf *buf, bool negative,
                              s_integer *dest);
sw buf_parse_integer_unsigned_bin (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_dec (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_hex (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_oct (s_buf *buf, s_integer *dest);
sw buf_parse_list_tag (s_buf *buf, s_tag *dest);
sw buf_parse_map (s_buf *buf, s_map *dest);
sw buf_parse_map_key (s_buf *buf, s_tag *dest);
sw buf_parse_map_key_str (s_buf *buf, s_tag *dest);
sw buf_parse_map_key_sym (s_buf *buf, s_tag *dest);
sw buf_parse_map_key_tag (s_buf *buf, s_tag *dest);
sw buf_parse_match (s_buf *buf, p_call *dest);
sw buf_parse_match_end (s_buf *buf, bool *dest);
sw buf_parse_module_name (s_buf *buf, const s_sym **dest);
sw buf_parse_module_name_sym_ignore (s_buf *buf);
sw buf_parse_new_tag (s_buf *buf, s_tag **dest);
sw buf_parse_paren_sym (s_buf *buf, const s_sym **dest);
sw buf_parse_pcall (s_buf *buf, p_call *dest);
sw buf_parse_pcall_args_paren (s_buf *buf, p_call *dest);
sw buf_parse_pcall_brackets (s_buf *buf, p_call *dest);
sw buf_parse_pcall_if (s_buf *buf, p_call *dest);
sw buf_parse_pcall_op (s_buf *buf, p_call *dest);
sw buf_parse_pcall_op_unary (s_buf *buf, p_call *dest);
sw buf_parse_pcall_paren (s_buf *buf, p_call *dest);
sw buf_parse_pcall_cast (s_buf *buf, p_call *dest);
sw buf_parse_pcallable (s_buf *buf, p_callable *dest);
sw buf_parse_plist (s_buf *buf, p_list *dest);
sw buf_parse_plist_paren (s_buf *buf, p_list *dest);
sw buf_parse_pointer (s_buf *buf, s_pointer *dest);
sw buf_parse_pstruct (s_buf *buf, p_struct *dest);
sw buf_parse_ptag (s_buf *buf, p_tag *dest);
sw buf_parse_ptr (s_buf *buf, u_ptr_w *dest);
sw buf_parse_ptr_free (s_buf *buf, u_ptr_w *dest);
sw buf_parse_pvar (s_buf *buf, p_var *dest);
sw buf_parse_quote (s_buf *buf, s_quote *dest);
sw buf_parse_ratio (s_buf *buf, s_ratio *dest);
sw buf_parse_special_operator (s_buf *buf, p_call *dest);
sw buf_parse_static_tag (s_buf *buf, s_tag *tag);
sw buf_parse_str (s_buf *buf, s_str *dest);
sw buf_parse_str_character (s_buf *buf, character *dest);
sw buf_parse_str_character_unicode (s_buf *buf, character *dest);
sw buf_parse_str_u8 (s_buf *buf, u8 *dest);
sw buf_parse_struct (s_buf *buf, s_struct *dest);
sw buf_parse_sym (s_buf *buf, const s_sym **dest);
sw buf_parse_sym_str (s_buf *buf, s_str *dest);
sw buf_parse_tag (s_buf *buf, s_tag *dest);
sw buf_parse_tag_array (s_buf *buf, s_tag *dest);
sw buf_parse_tag_do_block (s_buf *buf, s_tag *dest);
sw buf_parse_tag_bool (s_buf *buf, s_tag *dest);
sw buf_parse_tag_character (s_buf *buf, s_tag *dest);
sw buf_parse_tag_f32 (s_buf *buf, s_tag *dest);
sw buf_parse_tag_f64 (s_buf *buf, s_tag *dest);
sw buf_parse_tag_ident (s_buf *buf, s_tag *dest);
sw buf_parse_tag_ident_sym (s_buf *buf, s_tag *dest);
sw buf_parse_tag_integer (s_buf *buf, s_tag *dest);
sw buf_parse_tag_map (s_buf *buf, s_tag *map);
sw buf_parse_tag_match (s_buf *buf, s_tag *dest);
sw buf_parse_tag_module_name (s_buf *buf, s_tag *module_name);
sw buf_parse_tag_number (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall_access (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall_brackets (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall_if (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall_op (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall_op_unary (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcall_paren (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pcallable (s_buf *buf, s_tag *dest);
sw buf_parse_tag_plist (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pointer (s_buf *buf, s_tag *dest);
sw buf_parse_tag_primary (s_buf *buf, s_tag *dest);
sw buf_parse_tag_primary_2 (s_buf *buf, s_tag *dest);
sw buf_parse_tag_primary_3 (s_buf *buf, s_tag *dest);
sw buf_parse_tag_primary_4 (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pstruct (s_buf *buf, s_tag *dest);
sw buf_parse_tag_ptr (s_buf *buf, s_tag *dest);
sw buf_parse_tag_ptr_free (s_buf *buf, s_tag *dest);
sw buf_parse_tag_pvar (s_buf *buf, s_tag *dest);
sw buf_parse_tag_quote (s_buf *buf, s_tag *dest);
sw buf_parse_tag_special_operator (s_buf *buf, s_tag *dest);
sw buf_parse_tag_str (s_buf *buf, s_tag *dest);
sw buf_parse_tag_sym (s_buf *buf, s_tag *dest);
sw buf_parse_tag_time (s_buf *buf, s_tag *dest);
sw buf_parse_tag_tuple (s_buf *buf, s_tag *dest);
sw buf_parse_tag_unquote (s_buf *buf, s_tag *dest);
sw buf_parse_tag_void (s_buf *buf, s_tag *dest);
sw buf_parse_time (s_buf *buf, s_time *dest);
sw buf_parse_time_as_sw (s_buf *buf, s_time *dest);
sw buf_parse_time_as_tags (s_buf *buf, s_time *dest);
sw buf_parse_tuple (s_buf *buf, s_tuple *dest);
sw buf_parse_u64_hex (s_buf *buf, u64 *dest);
sw buf_parse_unquote (s_buf *buf, s_unquote *dest);
sw buf_parse_void (s_buf *buf, void *dest);
sw buf_parse_tag_ratio (s_buf *buf, s_tag *dest);
sw buf_peek_array_dimensions (s_buf *buf, s_array *dest);
sw buf_peek_ident (s_buf *buf, s_ident *dest);

#endif /* LIBKC3_BUF_PARSE_H */
