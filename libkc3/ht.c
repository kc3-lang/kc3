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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "ht.h"
#include "ht_item.h"

/* Returns true if data was added or is already present. */
bool ht_add (s_ht *ht, void *data)
{
  uw hash = ht->hash(data);
  return ht_add_hash(ht, data, hash);
}

/* Returns true if data was added or is already present. */
bool ht_add_hash (s_ht *ht, void *data, uw hash)
{
  s8 c;
  s_ht_item **item;
  s_ht_item  *item_new;
  assert(ht);
  assert(ht->size);
  assert(data);
  /* FIXME: lock / unlock */
  item = &ht->items[hash % ht->size];
  while (*item && (c = ht->compare(*item, data)) < 0)
    item = &(*item)->next;
  if (*item && ! c) {
    *item = ht_item_delete(ht, *item);
    ht->count--;
  }
  if (! (item_new = ht_item_new(ht, data, *item)))
    return false;
  *item = item_new;
  ht->count++;
  return true;
}

void ht_clean (s_ht *ht)
{
  uw i = 0;
  assert(ht);
  while (i < ht->size) {
    while (ht->items[i])
      ht->items[i] = ht_item_delete(ht, ht->items[i]);
    i++;
  }
  free(ht->items);
}

void * ht_get (s_ht *ht, void *data)
{
  uw hash = ht->hash(data);
  return ht_get_hash(ht, data, hash);
}

void * ht_get_hash (s_ht *ht, void *data, uw hash)
{
  sw c = -1;
  s_ht_item *item;
  item = ht->items[hash % ht->size];
  while (item && (c = ht->compare(item, data)) < 0)
    item = item->next;
  if (item && ! c)
    return item->data;
  return NULL;
}

s_ht * ht_init (s_ht *ht, const s_sym *type, uw size)
{
  s_ht tmp = {0};
  assert(ht);
  assert(type);
  assert(size);
  tmp.type = type;
  tmp.size = size;
  if (! (tmp.items = alloc(size * sizeof(s_ht_item *))))
    return NULL;
  *ht = tmp;
  return ht;
}
