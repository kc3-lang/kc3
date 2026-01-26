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
/** ht.h: Hash table of tags
 */
#ifndef LIBKC3_HT_H
#define LIBKC3_HT_H

#include "types.h"

/* Stack-allocation compatible functions, call ht_clean after use. */
void   ht_clean (s_ht *ht);
s_ht * ht_init (s_ht *ht, const s_sym *type, uw size);

/* Heap-allocation functions, call ht_delete after use. */
s_ht * ht_new_ref (s_ht *src);
void   ht_delete (s_ht *ht);

/* Observers. */
s_tag ** ht_get (s_ht *ht, s_tag *key, s_tag **dest);
s_tag ** ht_get_hash (s_ht *ht, s_tag *tag, uw hash, s_tag **dest);
bool     ht_has (s_ht *ht, const s_tag *key);

/* Operators. */
bool ht_add (s_ht *ht, s_tag *tag);
bool ht_add_hash (s_ht *ht, s_tag *tag, uw hash);

/* Iterator. */
s_ht_iterator * ht_iterator_init (s_ht_iterator *i, s_ht *ht);
s_tag **        ht_iterator_next (s_ht_iterator *i, s_tag **dest);

#endif /* LIBKC3_HT_H */
