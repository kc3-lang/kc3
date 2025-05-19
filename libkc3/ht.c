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
#include "compare.h"
#include "hash.h"
#include "ht.h"
#include "list.h"
#include "rwlock.h"
#include "tag.h"

/* Returns true if tag was added or is already present. */
bool ht_add (s_ht *ht, s_tag *tag)
{
  uw hash = ht->hash(tag);
  return ht_add_hash(ht, tag, hash);
}

/* Returns true if tag was added or is already present. */
bool ht_add_hash (s_ht *ht, s_tag *tag, uw hash)
{
  s8 c;
  s_list **item;
  s_list  *item_new;
  assert(ht);
  assert(ht->size);
  assert(tag);
#ifdef HAVE_PTHREAD
  rwlock_w(&ht->rwlock);
#endif
  item = ht->items + hash % ht->size;
  while (*item && (c = ht->compare(&(*item)->tag, tag)) < 0)
    item = &(*item)->next.data.list;
  if (*item && ! c) {
    *item = list_delete(*item);
    ht->count--;
  }
  if (! (item_new = list_new_tag_copy(tag, *item))) {
    err_puts("ht_add_hash: ht_item_new");
    assert(! "ht_add_hash: ht_item_new");
#ifdef HAVE_PTHREAD
    rwlock_unlock_w(&ht->rwlock);
#endif
    return false;
  }
  *item = item_new;
  ht->count++;
#ifdef HAVE_PTHREAD
  rwlock_unlock_w(&ht->rwlock);
#endif
  return true;
}

void ht_clean (s_ht *ht)
{
  uw i = 0;
  assert(ht);
#ifdef HAVE_PTHREAD
  rwlock_w(&ht->rwlock);
#endif
  while (i < ht->size) {
    while (ht->items[i])
      ht->items[i] = list_delete(ht->items[i]);
    i++;
  }
  free(ht->items);
#ifdef HAVE_PTHREAD
  rwlock_unlock_w(&ht->rwlock);
  rwlock_clean(&ht->rwlock);
#endif
}

s_tag * ht_get (s_ht *ht, s_tag *key, s_tag *dest)
{
  uw hash = ht->hash(key);
  return ht_get_hash(ht, key, hash, dest);
}

s_tag * ht_get_hash (s_ht *ht, s_tag *key, uw hash, s_tag *dest)
{
  sw c = -1;
  s_list *item;
#ifdef HAVE_PTHREAD
  rwlock_r(&ht->rwlock);
#endif
  item = ht->items[hash % ht->size];
  while (item && (c = ht->compare(&item->tag, key)) < 0)
    item = list_next(item);
  if (item && ! c)
    return tag_init_copy(dest, &item->tag);
#ifdef HAVE_PTHREAD
  rwlock_unlock_r(&ht->rwlock);
#endif
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
#ifdef HAVE_PTHREAD
  rwlock_init(&tmp.rwlock);
#endif
  *ht = tmp;
  return ht;
}
