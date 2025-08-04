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
/* Gen from set.c.in NAME=uw TYPE=uw */
#include "alloc.h"
#include "assert.h"
#include "compare.h"
#include "uw.h"
#include "set__uw.h"
#include "set_item__uw.h"

s_set_item__uw *
set_add__uw (s_set__uw *set, uw *data)
{
  uw hash;
  assert(set);
  assert(data);
  if (! uw_hash_uw(_Generic (*data,
                                 f32:  *data,
                                 f64:  *data,
                                 f128: *data,
                                 s8:   *data,
                                 s16:  *data,
                                 s32:  *data,
                                 s64:  *data,
                                 u8:   *data,
                                 u16:  *data,
                                 u32:  *data,
                                 u64:  *data,
                                 default: data), &hash)) {
    err_puts("set_add__uw: uw_hash_uw");
    assert(! "set_add__uw: uw_hash_uw");
    return NULL;
  }
  return set_add_h__uw(set, data, hash);
}

s_set_item__uw *
set_add_collision__uw
(s_set__uw *set,
 uw *data,
 uw hash,
 s_set_item__uw *item)
{
  s_set_item__uw *new_item;
  new_item = set_item_new__uw(data, hash, item->next);
  item->next = new_item;
  set->count++;
  set->collisions++;
  return new_item;
}

s_set_item__uw *
set_add_h__uw (s_set__uw *set, uw *data, uw hash)
{
  uw h;
  s_set_item__uw *i;
  assert(set);
  assert(data);
  if ((i = set_get_h__uw(set, data, hash)))
    return i;
  h = hash % set->max;
  if ((i = set->items[h]))
    return set_add_collision__uw(set, data, hash, i);
  if (! (i = set_item_new__uw(data, hash, NULL))) {
    err_puts("set_add_h__uw: set_item_new__uw");
    assert(! "set_add_h__uw: set_item_new__uw");
    return NULL;
  }
  set->items[h] = i;
  set->count++;
  return i;
}

void
set_clean__uw (s_set__uw *set)
{
  uw i;
  assert(set);
  for (i = 0; i < set->max; i++) {
    set_item_delete_all__uw(set->items[i]);
  }
  free(set->items);
}

s_set_item__uw *
set_get__uw (const s_set__uw *set, const uw *data)
{
  uw hash;
  assert(set);
  assert(data);
  if (! uw_hash_uw(_Generic (*data,
                                 f32:  *data,
                                 f64:  *data,
                                 f128: *data,
                                 s8:   *data,
                                 s16:  *data,
                                 s32:  *data,
                                 s64:  *data,
                                 u8:   *data,
                                 u16:  *data,
                                 u32:  *data,
                                 u64:  *data,
                                 default: data), &hash))
    return NULL;
  return set_get_h__uw(set, data, hash);
}

s_set_item__uw *
set_get_h__uw
(const s_set__uw *set,
 const uw *data,
 uw hash)
{
  s_set_item__uw *i;
  assert(set);
  assert(data);
  i = set_get_hash__uw(set, hash);
  while (i) {
    
    if (compare_uw(_Generic (i->data,
                                 f32:  i->data,
                                 f64:  i->data,
                                 f128: i->data,
                                 s8:   i->data,
                                 s16:  i->data,
                                 s32:  i->data,
                                 s64:  i->data,
                                 u8:   i->data,
                                 u16:  i->data,
                                 u32:  i->data,
                                 u64:  i->data,
                                 default: &i->data),
                       _Generic (*data,
                                 f32:  *data,
                                 f64:  *data,
                                 f128: *data,
                                 s8:   *data,
                                 s16:  *data,
                                 s32:  *data,
                                 s64:  *data,
                                 u8:   *data,
                                 u16:  *data,
                                 u32:  *data,
                                 u64:  *data,
                                 default: data)) == 0)
      return i;
    i = set_get_hash_next__uw(i);
  }
  return NULL;
}

s_set_item__uw *
set_get_hash__uw (const s_set__uw *set, uw hash)
{
  uw h;
  s_set_item__uw *i;
  assert(set);
  h = hash % set->max;
  i = set->items[h];
  while (i && i->hash != hash)
    i = i->next;
  return i;
}

s_set_item__uw *
set_get_hash_next__uw (const s_set_item__uw *item)
{
  s_set_item__uw *i;
  assert(item);
  i = item->next;
  while (i && i->hash != item->hash)
    i = i->next;
  return i;
}

bool *
set_has__uw
(const s_set__uw *set,
 const uw *data,
 bool *dest)
{
  uw hash;
  assert(set);
  assert(dest);
  if (! uw_hash_uw(_Generic (*data,
                                 f32:  *data,
                                 f64:  *data,
                                 f128: *data,
                                 s8:   *data,
                                 s16:  *data,
                                 s32:  *data,
                                 s64:  *data,
                                 u8:   *data,
                                 u16:  *data,
                                 u32:  *data,
                                 u64:  *data,
                                 default: data), &hash))
    return NULL;
  if (! set_get_h__uw(set, data, hash)) {
    *dest = false;
    return dest;
  }
  *dest = true;
  return dest;
}

s_set__uw *
set_init__uw (s_set__uw *set, uw max)
{
  s_set__uw tmp = {0};
  assert(set);
  assert(max > 0);
  tmp.max = max;
  tmp.items = alloc(max * sizeof(s_set_item__uw *));
  if (! tmp.items)
    return NULL;
  tmp.count = 0;
  tmp.collisions = 0;
  *set = tmp;
  return set;
}

bool
set_remove__uw (s_set__uw *set, const uw *data)
{
  s_set_item__uw *item;
  if ((item = set_get__uw(set, data)))
    return set_remove_item__uw(set, item);
  return false;
}

bool
set_remove_item__uw (s_set__uw *set, s_set_item__uw *item)
{
  sw h;
  s_set_item__uw *i;
  s_set_item__uw **j;
  s_set_item__uw *k;
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
  set_item_delete__uw(i);
  *j = k;
  set->count--;
  if (set->items[h])
    set->collisions--;
  return true;
}

s_set__uw *
set_resize__uw (s_set__uw *set, uw max)
{
  uw i;
  s_set_item__uw *item;
  s_set__uw n;
  if (set->max == max)
    return set;
  set_init__uw(&n, max);
  for (i = 0; i < set->max; i++) {
    item = set->items[i];
    while (item) {
      set_add_h__uw(&n, &item->data, item->hash);
      item = item->next;
    }
  }
  set_clean__uw(set);
  set->max = n.max;
  set->items = n.items;
  set->count = n.count;
  set->collisions = n.collisions;
  return set;
}
