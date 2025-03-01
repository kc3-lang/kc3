/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "ht.h"
#include "ht_item.h"

void ht_item_clean (s_ht *ht, s_ht_item *ht_item)
{
  assert(ht_item);
  ht->delete_ref(ht_item->data);
}

s_ht_item * ht_item_delete (s_ht *ht, s_ht_item *ht_item)
{
  s_ht_item *next;
  assert(ht);
  assert(ht_item);
  next = ht_item->next;
  ht_item_clean(ht, ht_item);
  free(ht_item);
  return next;
}

s_ht_item * ht_item_init (s_ht_item *ht_item, void *data,
                          s_ht_item *next)
{
  s_ht_item tmp = {0};
  assert(ht_item);
  assert(data);
  assert(next);
  tmp.data = data;
  tmp.next = next;
  *ht_item = tmp;
  return ht_item;
}
  
s_ht_item * ht_item_new (void *data, s_ht_item *next)
{
  s_ht_item *item;
  if (! (item = alloc(sizeof(s_ht_item))))
    return NULL;
  if (! ht_item_init(item, data, next)) {
    free(item);
    return NULL;
  }
  return item;
}
