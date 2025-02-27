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

/* Returns true if data was added or is already present. */
bool ht_add (s_ht *ht, void *data)
{
  uw hash = ht->hash(data);
  void *ref = ht->new_ref(data);
  return ht_add_hash(ht, ref, hash);
}

/* Returns true if data was added or is already present. */
bool ht_add_hash (s_ht *ht, void *data, uw hash)
{
  s8 c;
  uw index;
  s_ht_item *item;
  assert(ht);
  assert(data);
  /* FIXME: lock / unlock */
  index = hash % ht->count;
  item = ht->items[index];
  while (item && (c = ht->compare(item, data)) < 0)
    item = item->next;
  if (c == 0)
    return true;
  if (! (item = ht_item_new(data, ht->items[index])))
    return false;
  ht->items[index] = item;
  ht->count++;
  return true;
}

void ht_clean (s_ht *ht)
{
  assert(ht);
  free(ht->items);
}

void * ht_get (s_ht *ht, void *data)
{
  uw hash = ht->hash(data);
  return ht_get_hash(ht, data, hash);
}

void * ht_get_hash (s_ht *ht, void *data, uw hash)
{
  sw c;
  s_ht_item *item;
  item = ht->items[hash % ht->size];
  while (item && (c = ht->compare(item, data)) < 0)
    item = item->next;
  if (! c)
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
  tmp.items = alloc(size * sizeof(s_ht_item));
  *ht = tmp;
  return ht;
}
