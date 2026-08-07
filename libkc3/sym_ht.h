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
#ifndef LIBKC3_SYM_HT_H
#define LIBKC3_SYM_HT_H

#include "types.h"

/* Constructors. Call sym_ht_delete after use. */
void       sym_ht_delete (s_sym_ht *ht);
s_sym_ht * sym_ht_new (uw size);

/* Observers. */
const s_sym * sym_ht_find (s_sym_ht *ht, const s_str *str);

/* Operators. */
const s_sym * sym_ht_register (s_sym_ht *ht, const s_sym *sym,
                               s_sym *sym_free);

/* Item constructors. Call sym_ht_item_delete after use. */
s_sym_ht_item * sym_ht_item_delete (s_sym_ht_item *item);
s_sym_ht_item * sym_ht_item_new (uw hash_uw, const s_sym *sym,
                                 s_sym *sym_free, s_sym_ht_item *next);
                   
#endif /* LIBKC3_SYM_HT_H */
