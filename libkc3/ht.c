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
  s8 c = -1;
  s_list **item;
  s_list  *item_new;
  assert(ht);
  assert(ht->size);
  assert(tag);
#if HAVE_PTHREAD
  rwlock_w(&ht->rwlock);
#endif
  item = ht->items + hash % ht->size;
  while (*item && (c = ht->compare(&(*item)->tag, tag)) < 0)
    item = &(*item)->next.data.plist;
  if (*item && ! c) {
    *item = list_delete(*item);
    ht->count--;
  }
  if (! (item_new = list_new_tag_copy(tag, *item))) {
    err_puts("ht_add_hash: ht_item_new");
    assert(! "ht_add_hash: ht_item_new");
#if HAVE_PTHREAD
    rwlock_unlock_w(&ht->rwlock);
#endif
    return false;
  }
  *item = item_new;
  ht->count++;
#if HAVE_PTHREAD
  rwlock_unlock_w(&ht->rwlock);
#endif
  return true;
}

void ht_clean (s_ht *ht)
{
  uw i = 0;
  assert(ht);
  while (i < ht->size) {
    while (ht->items[i])
      ht->items[i] = list_delete(ht->items[i]);
    i++;
  }
  free(ht->items);
#if HAVE_PTHREAD
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
#if HAVE_PTHREAD
  rwlock_r(&ht->rwlock);
#endif
  item = ht->items[hash % ht->size];
  while (item && (c = ht->compare(&item->tag, key)) < 0)
    item = list_next(item);
  if (item && ! c) {
    if (! tag_init_copy(dest, &item->tag)) {
#if HAVE_PTHREAD
      rwlock_unlock_r(&ht->rwlock);
#endif
      return NULL;
    }
#if HAVE_PTHREAD
    rwlock_unlock_r(&ht->rwlock);
#endif
    return dest;
  }
#if HAVE_PTHREAD
  rwlock_unlock_r(&ht->rwlock);
#endif
  return NULL;
}

bool ht_has (s_ht *ht, const s_tag *key)
{
  uw hash = ht->hash(key);
  sw c = -1;
  s_list *item;
#if HAVE_PTHREAD
  rwlock_r(&ht->rwlock);
#endif
  item = ht->items[hash % ht->size];
  while (item && (c = ht->compare(&item->tag, key)) < 0) {
    if ((item = list_next(item)) && ! c)
      return true;
  }
#if HAVE_PTHREAD
  rwlock_unlock_r(&ht->rwlock);
#endif
  return false;
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
#if HAVE_PTHREAD
  rwlock_init(&tmp.rwlock);
#endif
  *ht = tmp;
  return ht;
}

s_ht_iterator * ht_iterator_init (s_ht_iterator *i, s_ht *ht)
{
  s_ht_iterator tmp = {0};
  if (! i || ! ht) {
    err_puts("ht_iterator_init: invalid argument");
    assert(! "ht_iterator_init: invalid argument");
    return NULL;
  }
  tmp.ht = ht;
  *i = tmp;
  return i;
}

s_tag ** ht_iterator_next (s_ht_iterator *i, s_tag **dest)
{
  if (! i || ! dest) {
    err_puts("ht_iterator_next: invalid argument");
    assert(! "ht_iterator_next: invalid argument");
    return NULL;
  }
  if (i->position == i->ht->size) {
    err_puts("ht_iterator_next: called after end");
    assert(! "ht_iterator_next: called after end");
    return NULL;
  }
  if (i->items)
    i->items = list_next(i->items);
  while (i->position < i->ht->size - 1 && ! i->items) {
    i->position++;
    i->items = i->ht->items[i->position];
  }
  if (i->position == i->ht->size || ! i->items) {
    err_puts("ht_iterator_next: reached end");
    assert(! "ht_iterator_next: reached end");
    return NULL;
  }
  *dest = &i->items->tag;
  return dest;
}
