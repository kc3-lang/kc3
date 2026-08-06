/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "alloc.h"
#include "assert.h"
#include "rwlock.h"
#include "str.h"
#include "sym.h"
#include "sym_ht.h"

#define SYM_HT_SIZE_DEFAULT 1024

const s_sym * sym_ht_add (s_sym_ht *ht, const s_sym *sym,
                          s_sym *sym_free)
{
  uw h;
  s_sym_ht_item *i;
  s_sym_ht_item **item;
  uw pos;
  h = str_hash_uw(&sym->str);
  pos = h % ht->size;
  item = ht->item + pos;
  if ((i = *item)) {
    while (i) {
      if (i->hash_uw == h)
        return i->sym;
      i = i->next;
    }
  }
  if (! (i = sym_ht_item_new(h, sym, sym_free, *item)))
    return NULL;
  *item = i;
  return sym;
}

void sym_ht_delete (s_sym_ht *ht)
{
  uw i;
  i = 0;
  while (i < ht->size) {
    while (ht->item[i]) {
      ht->item[i] = sym_ht_item_delete(ht->item[i]);
      ht->count--;
    }
    i++;
  }
  alloc_free(ht);
}

const s_sym * sym_ht_find (s_sym_ht *ht, const s_str *str)
{
  uw h;
  s_sym_ht_item *item;
  uw pos;
  h = str_hash_uw(str);
  pos = h % ht->size;
  item = ht->item[pos];
  while (item && item->hash_uw != h)
    item = item->next;
  if (item)
    return item->sym;
  return NULL;
}

s_sym_ht * sym_ht_new (uw size)
{
  s_sym_ht *ht;
  if (! size)
    size = SYM_HT_SIZE_DEFAULT;
  if (! (ht = alloc(sizeof(s_sym_ht))))
    return NULL;
  ht->size = size;
  ht->item = alloc(size * sizeof(s_sym_ht_item *));
#if HAVE_PTHREAD
  rwlock_init(&ht->rwlock);
#endif
  return ht;
}

s_sym_ht_item * sym_ht_item_new (uw hash_uw, const s_sym *sym,
                                 s_sym *sym_free, s_sym_ht_item *next)
{
  s_sym_ht_item *item;
  assert(sym);
  if (! (item = alloc(sizeof(s_sym_ht_item))))
    return NULL;
  item->hash_uw = hash_uw;
  item->sym = sym;
  item->sym_free = sym_free;
  item->next = next;
  return item;
}

s_sym_ht_item * sym_ht_item_delete (s_sym_ht_item *item)
{
  s_sym_ht_item *next = NULL;
  if (item) {
    if (item->sym_free)
      sym_delete(item->sym_free);
    next = item->next;
    alloc_free(item);
  }
  return next;
}
