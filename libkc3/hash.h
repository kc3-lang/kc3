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
#ifndef LIBKC3_HASH_H
#define LIBKC3_HASH_H

#include "types.h"

#define HASH_UPDATE_PROTOTYPE(type)                           \
  bool hash_update_##type (t_hash *hash, type x)

void hash_clean (t_hash *hash);
void hash_init (t_hash *hash);
uw   hash_tag (const s_tag *tag);
uw   hash_to_uw (t_hash *hash);
u64  hash_to_u64 (t_hash *hash);
bool hash_update (t_hash *hash, const void *data, uw size);
bool hash_update_1 (t_hash *hash, const char *p);
bool hash_update_array (t_hash *hash, const s_array *a);
bool hash_update_do_block (t_hash *hash, const s_do_block *do_block);
bool hash_update_bool (t_hash *hash, bool b);
bool hash_update_call (t_hash *hash, const s_call *call);
bool hash_update_callable (t_hash *hash, const s_callable *call);
bool hash_update_cfn (t_hash *hash, const s_cfn *cfn);
HASH_UPDATE_PROTOTYPE(char);
HASH_UPDATE_PROTOTYPE(character);
bool hash_update_complex (t_hash *hash, const s_complex *c);
bool hash_update_cow (t_hash *hash, s_cow *cow);
HASH_UPDATE_PROTOTYPE(f32);
HASH_UPDATE_PROTOTYPE(f64);
HASH_UPDATE_PROTOTYPE(f80);
#if HAVE_FLOAT128
HASH_UPDATE_PROTOTYPE(f128);
#endif
bool hash_update_fact (t_hash *hash, const s_fact *fact);
bool hash_update_fn (t_hash *hash, const s_fn *fn);
bool hash_update_fn_clauses (t_hash *hash, const s_fn_clause *clauses);
bool hash_update_ident (t_hash *hash, const s_ident *ident);
bool hash_update_integer (t_hash *hash, const s_integer *i);
bool hash_update_list (t_hash *hash, const s_list *list);
bool hash_update_map (t_hash *hash, const s_map *map);
bool hash_update_pcomplex (t_hash *hash, const p_complex *c);
bool hash_update_pfacts (t_hash *hash, const s_facts *facts);
bool hash_update_plist (t_hash *hash, const p_list *plist);
bool hash_update_pointer (t_hash *hash, const s_pointer *pointer);
bool hash_update_psym (t_hash *hash, p_sym const *psym);
bool hash_update_ptag (t_hash *hash, const p_tag *ptag);
bool hash_update_ptr (t_hash *hash, const u_ptr_w *ptr);
bool hash_update_ptr_free (t_hash *hash, const u_ptr_w *ptr_free);
bool hash_update_quote (t_hash *hash, const s_quote *quote);
bool hash_update_ratio (t_hash *hash, const s_ratio *ratio);
HASH_UPDATE_PROTOTYPE(s8);
HASH_UPDATE_PROTOTYPE(s16);
HASH_UPDATE_PROTOTYPE(s32);
HASH_UPDATE_PROTOTYPE(s64);
HASH_UPDATE_PROTOTYPE(sw);
bool hash_update_str (t_hash *hash, const s_str *str);
bool hash_update_struct (t_hash *hash, const s_struct *s);
bool hash_update_struct_type (t_hash *hash, const s_struct_type *st);
bool hash_update_sym (t_hash *hash, const s_sym *sym);
bool hash_update_tag (t_hash *hash, const s_tag *tag);
bool hash_update_time (t_hash *hash, const s_time *time);
bool hash_update_tuple (t_hash *hash, const s_tuple *tuple);
HASH_UPDATE_PROTOTYPE(u8);
HASH_UPDATE_PROTOTYPE(u16);
HASH_UPDATE_PROTOTYPE(u32);
HASH_UPDATE_PROTOTYPE(u64);
bool hash_update_unquote (t_hash *hash, const s_unquote *unquote);
HASH_UPDATE_PROTOTYPE(uw);
bool hash_update_var (t_hash *hash, const s_var *var);
bool hash_update_void (t_hash *hash);

#endif /* LIBKC3_HASH_H */
