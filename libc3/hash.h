/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef HASH_H
#define HASH_H

#include "types.h"

#define HASH_UPDATE_PROTOTYPE(type)                           \
  void hash_update_##type (t_hash *hash, type x)

void hash_clean (t_hash *hash);
void hash_init (t_hash *hash);
uw   hash_to_uw (t_hash *hash);
u64  hash_to_u64 (t_hash *hash);
void hash_update (t_hash *hash, const void *data, uw size);
void hash_update_1 (t_hash *hash, const s8 *p);
void hash_update_bool (t_hash *hash, e_bool b);
void hash_update_call (t_hash *hash, const s_call *call);
void hash_update_fact (t_hash *hash, const s_fact *fact);
HASH_UPDATE_PROTOTYPE(f32);
HASH_UPDATE_PROTOTYPE(f64);
void hash_update_ident (t_hash *hash, const s_ident *ident);
void hash_update_integer (t_hash *hash, const s_integer *i);
void hash_update_list (t_hash *hash, const s_list *list);
void hash_update_ptag (t_hash *hash, const p_tag ptag);
void hash_update_quote (t_hash *hash, const p_quote x);
HASH_UPDATE_PROTOTYPE(s8);
HASH_UPDATE_PROTOTYPE(s16);
HASH_UPDATE_PROTOTYPE(s32);
HASH_UPDATE_PROTOTYPE(s64);
void hash_update_str (t_hash *hash, const s_str *src);
void hash_update_sym (t_hash *hash, const s_sym *sym);
void hash_update_tag (t_hash *hash, const s_tag *tag);
void hash_update_tuple (t_hash *hash, const s_tuple *tuple);
HASH_UPDATE_PROTOTYPE(u8);
HASH_UPDATE_PROTOTYPE(u16);
HASH_UPDATE_PROTOTYPE(u32);
HASH_UPDATE_PROTOTYPE(u64);

#endif /* HASH_H */
