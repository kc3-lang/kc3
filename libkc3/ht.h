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
/** ht.h: Hash tables
 */
#ifndef LIBKC3_HT_H
#define LIBKC3_HT_H

#include "types.h"

/* Stack-allocation compatible functions, call ht_clean after use. */
void   ht_clean (s_ht *ht);
s_ht * ht_init (s_ht *ht, const s_sym *type, uw size);

/* Observers. */
void * ht_get (s_ht *ht, void *data);
void * ht_get_hash (s_ht *ht, void *data, uw hash);

/* Operators. */
void * ht_add (s_ht *ht, void *data);
void * ht_add_hash (s_ht *ht, void *data, uw hash);

#endif /* LIBKC3_HT_H */
