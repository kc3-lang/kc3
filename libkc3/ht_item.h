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
/** ht_item.h: Hash table item (linked list)
 */
#ifndef LIBKC3_HT_ITEM_H
#define LIBKC3_HT_ITEM_H

#include "types.h"

/* Stack-allocation compatible functions, call ht_item_clean
   after use. */ 
void        ht_item_clean (s_ht *ht, s_ht_item *item);
s_ht_item * ht_item_init (s_ht_item *item, void *data, s_ht_item *next);

/* Heap allocation compatible functions, call ht_item_delete
   after use. */
s_ht_item * ht_item_delete (s_ht *ht, s_ht_item *item);
s_ht_item * ht_item_new (void *data, s_ht_item *next);

#endif /* LIBKC3_HT_ITEM_H */
