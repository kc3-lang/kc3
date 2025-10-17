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
/* Gen from set.c.in NAME=fact TYPE=s_fact * */
#include "alloc.h"
#include "assert.h"
#include "compare.h"
#include <config.h>
#include "fact.h"
#include "set__fact.h"
#include "set_item__fact.h"

s_set_item__fact *
set_add__fact (s_set__fact *set, s_fact * data)
{
  uw hash;
  assert(set);
  assert(data);
  if (! fact_hash_uw(data, &hash)) {
    err_puts("set_add__fact: fact_hash_uw");
    assert(! "set_add__fact: fact_hash_uw");
    return NULL;
  }
  return set_add_h__fact(set, data, hash);
}

s_set_item__fact *
set_add_collision__fact
(s_set__fact *set,
 s_fact * data,
 uw hash,
 s_set_item__fact *item)
{
  s_set_item__fact *new_item;
  new_item = set_item_new__fact(data, hash, item->next);
  item->next = new_item;
  set->count++;
  set->collisions++;
  return new_item;
}

s_set_item__fact *
set_add_h__fact (s_set__fact *set, s_fact * data, uw hash)
{
  uw h;
  s_set_item__fact *item;
  assert(set);
  assert(data);
  if ((item = set_get_h__fact(set, data, hash)))
    return item;
  h = hash % set->max;
  if ((item = set->items[h]))
    return set_add_collision__fact(set, data, hash, item);
  if (! (item = set_item_new__fact(data, hash, NULL))) {
    err_puts("set_add_h__fact: set_item_new__fact");
    assert(! "set_add_h__fact: set_item_new__fact");
    return NULL;
  }
  set->items[h] = item;
  set->count++;
  return item;
}

void
set_clean__fact (s_set__fact *set)
{
  uw i;
  assert(set);
  for (i = 0; i < set->max; i++) {
    set_item_delete_all__fact(set->items[i]);
  }
  free(set->items);
}

s_set_item__fact *
set_get__fact (const s_set__fact *set, const s_fact * data)
{
  uw hash;
  assert(set);
  assert(data);
  if (! fact_hash_uw(data, &hash))
    return NULL;
  return set_get_h__fact(set, data, hash);
}

s_set_item__fact *
set_get_h__fact
(const s_set__fact *set,
 const s_fact * data,
 uw hash)
{
  s_set_item__fact *item;
  assert(set);
  assert(data);
  item = set_get_hash__fact(set, hash);
  while (item) {
    if (! compare_fact(_Generic(item->data,
                                  f32:  item->data,
                                  f64:  item->data,
                                  f128: item->data,
                                  s8:   item->data,
                                  s16:  item->data,
                                  s32:  item->data,
                                  s64:  item->data,
#if HAVE_GENERIC_SW_UW
                                  sw:   item->data,
#endif
                                  u8:   item->data,
                                  u16:  item->data,
                                  u32:  item->data,
                                  u64:  item->data,
#if HAVE_GENERIC_SW_UW
                                  uw:   item->data,
#endif
                                  default: &item->data),
                         data))
      return item;
    item = set_get_hash_next__fact(item);
  }
  return NULL;
}

s_set_item__fact *
set_get_hash__fact (const s_set__fact *set, uw hash)
{
  uw h;
  s_set_item__fact *item;
  assert(set);
  h = hash % set->max;
  item = set->items[h];
  while (item && item->hash != hash)
    item = item->next;
  return item;
}

s_set_item__fact *
set_get_hash_next__fact (const s_set_item__fact *item)
{
  s_set_item__fact *i;
  uw hash;
  assert(item);
  hash = item->hash;
  i = item->next;
  while (i && i->hash != hash)
    i = i->next;
  return i;
}

bool *
set_has__fact
(const s_set__fact *set,
 const s_fact * data,
 bool *dest)
{
  uw hash;
  assert(set);
  assert(dest);
  if (! fact_hash_uw(data, &hash))
    return NULL;
  if (! set_get_h__fact(set, data, hash)) {
    *dest = false;
    return dest;
  }
  *dest = true;
  return dest;
}

s_set__fact *
set_init__fact
(s_set__fact *set,
 uw max)
{
  s_set__fact tmp = {0};
  assert(set);
  assert(max > 0);
  tmp.max = max;
  tmp.items = alloc(max * sizeof(s_set_item__fact *));
  if (! tmp.items)
    return NULL;
  tmp.count = 0;
  tmp.collisions = 0;
  *set = tmp;
  return set;
}

bool
set_remove__fact (s_set__fact *set, const s_fact * data)
{
  s_set_item__fact *item;
  if ((item = set_get__fact(set, data)))
    return set_remove_item__fact(set, item);
  return false;
}

void
set_remove_all__fact (s_set__fact *set)
{
  uw i = 0;
  assert(set);
  while (i < set->max) {
    set_item_delete_all__fact(set->items[i]);
    i++;
  }
}

bool
set_remove_item__fact (s_set__fact *set, s_set_item__fact *item)
{
  sw h;
  s_set_item__fact *i;
  s_set_item__fact **j;
  s_set_item__fact *k;
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
  set_item_delete__fact(i);
  *j = k;
  set->count--;
  if (set->items[h])
    set->collisions--;
  return true;
}

s_set__fact *
set_resize__fact (s_set__fact *set, uw max)
{
  uw i;
  s_set_item__fact *item;
  s_set__fact n;
  if (set->max == max)
    return set;
  set_init__fact(&n, max);
  for (i = 0; i < set->max; i++) {
    item = set->items[i];
    while (item) {
      set_add_h__fact(&n, _Generic(item->data,
                                       f32:  item->data,
                                       f64:  item->data,
                                       f128: item->data,
                                       s8:   item->data,
                                       s16:  item->data,
                                       s32:  item->data,
                                       s64:  item->data,
#if HAVE_GENERIC_SW_UW
                                       sw:   item->data,
#endif
                                       u8:   item->data,
                                       u16:  item->data,
                                       u32:  item->data,
                                       u64:  item->data,
#if HAVE_GENERIC_SW_UW
                                       uw:   item->data,
#endif
                                       default: &item->data),
                        item->hash);
      item = item->next;
    }
  }
  set_clean__fact(set);
  set->max = n.max;
  set->items = n.items;
  set->count = n.count;
  set->collisions = n.collisions;
  return set;
}
