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
#include "mutex.h"
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
  if (! (item_new = list_new_tag_copy(tag, *item))) {
    err_puts("ht_add_hash: ht_item_new");
    assert(! "ht_add_hash: ht_item_new");
#if HAVE_PTHREAD
    rwlock_unlock_w(&ht->rwlock);
#endif
    return false;
  }
  if (*item && ! c) {
    *item = list_delete(*item);
    ht->count--;
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

void ht_delete (s_ht *ht)
{
  assert(ht);
  if (ht->ref_count <= 0) {
    err_puts("ht_delete: invalid reference count");
    assert(! "ht_delete: invalid reference count");
    return;
  }
#if HAVE_PTHREAD
  mutex_lock(&ht->ref_count_mutex);
#endif
  ht->ref_count--;
  if (ht->ref_count) {
#if HAVE_PTHREAD
    mutex_unlock(&ht->ref_count_mutex);
#endif
    return;
  }
#if HAVE_PTHREAD
  mutex_unlock(&ht->ref_count_mutex);
#endif
  ht_clean(ht);
  free(ht);
}

s_tag ** ht_get (s_ht *ht, s_tag *key, s_tag **dest)
{
  uw hash = ht->hash(key);
  return ht_get_hash(ht, key, hash, dest);
}

s_tag ** ht_get_hash (s_ht *ht, s_tag *key, uw hash, s_tag **dest)
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
    *dest = &item->tag;
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
  uw hash;
  sw c = -1;
  s_list *item;
  hash = ht->hash(key);
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
  assert(ht);
  assert(type);
  assert(size);
  *ht = (s_ht) {0};
  ht->type = type;
  ht->size = size;
  if (! (ht->items = alloc(size * sizeof(s_list *))))
    return NULL;
  ht->compare = compare_tag;
  ht->hash = hash_tag;
  ht->ref_count = 1;
#if HAVE_PTHREAD
  rwlock_init(&ht->rwlock);
  mutex_init(&ht->ref_count_mutex);
#endif
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
  if (i->position >= i->ht->size) {
    err_puts("ht_iterator_next: called after end");
    assert(! "ht_iterator_next: called after end");
    return NULL;
  }
  if (! i->next) {
    i->next = true;
    if ((i->items = i->ht->items[i->position]))
      goto ok;
  }
  else
    if (i->items)
      i->items = list_next(i->items);
  while (i->position < i->ht->size - 1 && ! i->items) {
    i->position++;
    i->items = i->ht->items[i->position];
  }
  if (! i->items && i->position >= i->ht->size) {
    err_write_1("ht_iterator_next: reached end (position=");
    err_inspect_uw_decimal(i->position);
    err_write_1(", size=");
    err_inspect_uw_decimal(i->ht->size);
    err_write_1(", count=");
    err_inspect_uw_decimal(i->ht->count);
    err_puts(")");
    assert(! "ht_iterator_next: reached end");
    return NULL;
  }
 ok:
  *dest = &i->items->tag;
  return dest;
}

s_ht * ht_new_ref (s_ht *src)
{
  assert(src);
  if (src->ref_count <= 0) {
    err_puts("ht_new_ref: invalid reference count");
    assert(! "ht_new_ref: invalid reference count");
    return NULL;
  }
#if HAVE_PTHREAD
  mutex_lock(&src->ref_count_mutex);
#endif
  src->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&src->ref_count_mutex);
#endif
  return src;
}
