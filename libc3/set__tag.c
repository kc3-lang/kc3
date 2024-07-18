/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
/* Gen from set.c.in NAME=tag TYPE=s_tag */
#include "alloc.h"
#include "assert.h"
#include "compare.h"
#include "tag.h"
#include "set__tag.h"
#include "set_item__tag.h"

s_set_item__tag *
set_add__tag (s_set__tag *set, const s_tag *data)
{
  uw hash;
  assert(set);
  assert(data);
  if (! tag_hash_uw(data, &hash)) {
    err_puts("set_add__tag: tag_hash_uw");
    assert(! "set_add__tag: tag_hash_uw");
    return NULL;
  }
  return set_add_h__tag(set, data, hash);
}

s_set_item__tag *
set_add_collision__tag (s_set__tag *set, const s_tag *data, uw hash, s_set_item__tag *item)
{
  s_set_item__tag *new_item;
  new_item = set_item_new__tag(data, hash, item->next);
  item->next = new_item;
  set->count++;
  set->collisions++;
  return new_item;
}

s_set_item__tag *
set_add_h__tag (s_set__tag *set, const s_tag *data, uw hash)
{
  uw h;
  s_set_item__tag *i;
  assert(set);
  assert(data);
  if ((i = set_get_h__tag(set, data, hash)))
    return i;
  h = hash % set->max;
  if ((i = set->items[h]))
    return set_add_collision__tag(set, data, hash, i);
  if (! (i = set_item_new__tag(data, hash, NULL))) {
    err_puts("set_add_h__tag: set_item_new__tag");
    assert(! "set_add_h__tag: set_item_new__tag");
    return NULL;
  }
  set->items[h] = i;
  set->count++;
  return i;
}
                 
void
set_clean__tag (s_set__tag *set)
{
  uw i;
  assert(set);
  for (i = 0; i < set->max; i++) {
    set_item_delete_all__tag(set->items[i]);
  }
  free(set->items);
}

s_set_item__tag *
set_get__tag (const s_set__tag *set, const s_tag *data)
{
  uw hash;
  assert(set);
  assert(data);
  if (! tag_hash_uw(data, &hash))
    return NULL;
  return set_get_h__tag(set, data, hash);
}

s_set_item__tag *
set_get_h__tag (const s_set__tag *set, const s_tag *data, uw hash)
{
  s_set_item__tag *i;
  assert(set);
  assert(data);
  i = set_get_hash__tag(set, hash);
  while (i) {
    if (compare_tag(&i->data, data) == 0)
      return i;
    i = set_get_hash_next__tag(i);
  }
  return NULL;
}

s_set_item__tag *
set_get_hash__tag (const s_set__tag *set, uw hash)
{
  uw h;
  s_set_item__tag *i;
  assert(set);
  h = hash % set->max;
  i = set->items[h];
  while (i && i->hash != hash)
    i = i->next;
  return i;
}

s_set_item__tag *
set_get_hash_next__tag (const s_set_item__tag *item)
{
  s_set_item__tag *i;
  assert(item);
  i = item->next;
  while (i && i->hash != item->hash)
    i = i->next;
  return i;
}

s_set__tag *
set_init__tag (s_set__tag *set, uw max)
{
  s_set__tag tmp = {0};
  assert(set);
  assert(max > 0);
  tmp.max = max;
  tmp.items = alloc(max * sizeof(s_set_item__tag *));
  if (! tmp.items)
    return NULL;
  tmp.count = 0;
  tmp.collisions = 0;
  *set = tmp;
  return set;
}

bool
set_remove__tag (s_set__tag *set, const s_tag *data)
{
  s_set_item__tag *item;
  if ((item = set_get__tag(set, data)))
    return set_remove_item__tag(set, item);
  return false;
}

bool
set_remove_item__tag (s_set__tag *set, s_set_item__tag *item)
{
  sw h;
  s_set_item__tag *i;
  s_set_item__tag **j;
  s_set_item__tag *k;
  assert(set);
  if (! item)
    return false;
  h = item->hash % set->max;
  j = set->items + h;
  while (*j && *j != item)
    j = &(*j)->next;
  if (!*j)
    return false;
  i = *j;
  k = i->next;
  set_item_delete__tag(i);
  *j = k;
  set->count--;
  if (set->items[h])
    set->collisions--;
  return true;
}

s_set__tag *
set_resize__tag (s_set__tag *set, uw max)
{
  uw i;
  s_set_item__tag *item;
  s_set__tag n;
  if (set->max == max)
    return set;
  set_init__tag(&n, max);
  for (i = 0; i < set->max; i++) {
    item = set->items[i];
    while (item) {
      set_add_h__tag(&n, &item->data, item->hash);
      item = item->next;
    }
  }
  set_clean__tag(set);
  set->max = n.max;
  set->items = n.items;
  set->count = n.count;
  set->collisions = n.collisions;
  return set;
}
