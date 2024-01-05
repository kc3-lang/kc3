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
#ifndef LIBC3_COMPARE_H
#define LIBC3_COMPARE_H

#include "types.h"

#define COMPARE_PROTOTYPE(type)                     \
  s8 compare_##type (type a, type b)

s8 compare_array (const s_array *a, const s_array *b);
s8 compare_bool (bool a, bool b);
s8 compare_call (const s_call *a, const s_call *b);
s8 compare_cfn (const s_cfn *a, const s_cfn *b);
s8 compare_character (character a, character b);
COMPARE_PROTOTYPE(f32);
COMPARE_PROTOTYPE(f64);
s8 compare_fact (const s_fact *a, const s_fact *b);
s8 compare_fact_pos (const s_fact *a, const s_fact *b);
s8 compare_fact_osp (const s_fact *a, const s_fact *b);
s8 compare_fact_unbound_var_count (const s_fact *a,
                                   const s_fact *b);
s8 compare_fn (const s_fn *a, const s_fn *b);
s8 compare_fn_clause (const s_fn_clause *a, const s_fn_clause *b);
s8 compare_ident (const s_ident *a, const s_ident *b);
s8 compare_integer (const s_integer *a, const s_integer *b);
s8 compare_integer_s64 (const s_integer *a, s64 b);
s8 compare_integer_u64 (const s_integer *a, u64 b);
s8 compare_list (const s_list *a, const s_list *b);
s8 compare_map (const s_map *a, const s_map *b);
s8 compare_ptag (const p_tag a, const p_tag b);
s8 compare_ptr (const void *a, const void *b);
s8 compare_quote (const s_quote *a, const s_quote *b);
COMPARE_PROTOTYPE(s8);
COMPARE_PROTOTYPE(s16);
COMPARE_PROTOTYPE(s32);
COMPARE_PROTOTYPE(s64);
COMPARE_PROTOTYPE(sw);
s8 compare_str (const s_str *a, const s_str *b);
s8 compare_struct (const s_struct *a, const s_struct *b);
s8 compare_struct_type (const s_struct_type *a, const s_struct_type *b);
s8 compare_sym (const s_sym *a, const s_sym *b);
s8 compare_tag (const s_tag *a, const s_tag *b);
s8 compare_tuple (const s_tuple *a, const s_tuple *b);
COMPARE_PROTOTYPE(u8);
COMPARE_PROTOTYPE(u16);
COMPARE_PROTOTYPE(u32);
COMPARE_PROTOTYPE(u64);
COMPARE_PROTOTYPE(uw);

#endif /* LIBC3_COMPARE_H */
