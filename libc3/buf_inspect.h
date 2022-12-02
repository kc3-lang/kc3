/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
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
#ifndef BUF_INSPECT_H
#define BUF_INSPECT_H

#include "types.h"

#define BUF_INSPECT_U64_HEX_SIZE 16
#define BUF_INSPECT_UW_HEX_SIZE (sizeof(uw) / 4)

sw buf_inspect_bool (s_buf *buf, e_bool b);
sw buf_inspect_bool_size (e_bool b);
sw buf_inspect_call (s_buf *buf, const s_call *call);
sw buf_inspect_call_args (s_buf *buf, const s_list *args);
sw buf_inspect_call_size (const s_call *call);
sw buf_inspect_character (s_buf *buf, character c);
sw buf_inspect_character_size (character c);
sw buf_inspect_f32 (s_buf *buf, f32 x);
sw buf_inspect_f32_size (f32 x);
sw buf_inspect_f64 (s_buf *buf, f64 x);
sw buf_inspect_f64_size (f64 x);
sw buf_inspect_fact (s_buf *buf, const s_fact *fact);
sw buf_inspect_fact_size (const s_fact *fact);
sw buf_inspect_fact_spec (s_buf *buf, p_facts_spec spec);
sw buf_inspect_fn (s_buf *buf, const s_fn *fn);
sw buf_inspect_fn_size (const s_fn *fn);
sw buf_inspect_ident (s_buf *buf, const s_ident *ident);
sw buf_inspect_ident_size (const s_ident *ident);
sw buf_inspect_integer (s_buf *buf, const s_integer *x);
sw buf_inspect_integer_size (const s_integer *x);
sw buf_inspect_list (s_buf *buf, const s_list *list);
sw buf_inspect_list_size (const s_list *list);
sw buf_inspect_ptag (s_buf *buf, p_tag ptag);
sw buf_inspect_ptag_size (p_tag ptag);
sw buf_inspect_quote (s_buf *buf, p_quote quote);
sw buf_inspect_quote_size (p_quote quote);
sw buf_inspect_s8 (s_buf *buf, s8 i);
sw buf_inspect_s8_size (s8 i);
sw buf_inspect_s16 (s_buf *buf, s16 i);
sw buf_inspect_s16_size (s16 i);
sw buf_inspect_s32 (s_buf *buf, s32 i);
sw buf_inspect_s32_size (s32 i);
sw buf_inspect_s64 (s_buf *buf, s64 i);
sw buf_inspect_s64_size (s64 i);
sw buf_inspect_str (s_buf *buf, const s_str *str);
sw buf_inspect_str_size (const s_str *str);
sw buf_inspect_str_character (s_buf *buf, character c);
sw buf_inspect_str_character_size (character c);
sw buf_inspect_sym (s_buf *buf, const s_sym *sym);
sw buf_inspect_sym_size (const s_sym *sym);
sw buf_inspect_tag (s_buf *buf, const s_tag *tag);
sw buf_inspect_tag_size (const s_tag *tag);
sw buf_inspect_tuple (s_buf *buf, const s_tuple *tuple);
sw buf_inspect_tuple_size (const s_tuple *tuple);
sw buf_inspect_u8 (s_buf *buf, u8 i);
sw buf_inspect_u8_size (u8 i);
sw buf_inspect_u16 (s_buf *buf, u16 i);
sw buf_inspect_u16_size (u16 i);
sw buf_inspect_u32 (s_buf *buf, u32 i);
sw buf_inspect_u32_size (u32 i);
sw buf_inspect_u32_hex (s_buf *buf, u32 i);
sw buf_inspect_u32_hex_size (u32 i);
sw buf_inspect_u64_hex (s_buf *buf, u64 i);
sw buf_inspect_u64_hex_size (u64 i);
sw buf_inspect_u64 (s_buf *buf, u64 i);
sw buf_inspect_u64_size (u64 i);
sw buf_inspect_uw_hex (s_buf *buf, uw i);
sw buf_inspect_var (s_buf *buf, const s_tag *var);

#endif /* BUF_INSPECT_H */
