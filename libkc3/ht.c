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
#include "list.h"

/* Returns true if tag was added or is already present. */
s_tag * ht_add (s_ht *ht, s_tag *tag)
{
  uw hash = ht->hash(tag);
  return ht_add_hash(ht, tag, hash);
}

/* Returns true if tag was added or is already present. */
s_tag * ht_add_hash (s_ht *ht, s_tag *tag, uw hash)
{
  s8 c;
  s_list **item;
  s_list  *item_new;
  assert(ht);
  assert(ht->size);
  assert(tag);
  /* FIXME: lock / unlock */
  item = ht->items + hash % ht->size;
  while (*item && (c = ht->compare((*item)->tag, tag)) < 0)
    item = &(*item)->next;
  if (*item && ! c) {
    *item = ht_item_delete(ht, *item);
    ht->count--;
  }
  if (! (item_new = ht_item_new(ht, tag, *item))) {
    err_puts("ht_add_hash: ht_item_new");
    assert(! "ht_add_hash: ht_item_new");
    return NULL;
  }
  *item = item_new;
  ht->count++;
  return item_new->tag;
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

s_tag * ht_get (s_ht *ht, s_tag *tag)
{
  uw hash = ht->hash(tag);
  return ht_get_hash(ht, tag, hash);
}

s_tag * ht_get_hash (s_ht *ht, s_tag *tag, uw hash)
{
  sw c = -1;
  s_list *item;
  item = ht->items[hash % ht->size];
  while (item && (c = ht->compare(item->tag, tag)) < 0)
    item = item->next;
  if (item && ! c)
    return item->tag;
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
  if (! (tmp.items = alloc(size * sizeof(s_list *))))
    return NULL;
  tmp.compare = compare_tag;
  tmp.hash = hash_tag;
  tmp.init_copy = tag_init_copy;
  tmp.clean = tag_clean;
  *ht = tmp;
  return ht;
}
