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
#include "compare.h"
#include "primehash.h"
#include "rwlock.h"
#include "str.h"
#include "sym.h"
#include "sym_ht.h"

#define SYM_HT_SIZE(ht) (1 << ht->size_exp)

#define SYM_HT_SIZE_EXP_DEFAULT 20

#define SYM_HT_MASK(ht) (SYM_HT_SIZE(ht) - 1)

void sym_ht_delete (s_sym_ht *ht)
{
  uw i;
  i = 0;
  while (i < SYM_HT_SIZE(ht)) {
    while (ht->item[i]) {
      ht->item[i] = sym_ht_item_delete(ht->item[i]);
      ht->count--;
    }
    i++;
  }
#if HAVE_PTHREAD
  rwlock_clean(&ht->rwlock);
#endif
  alloc_free(ht->item);
  alloc_free(ht);
}

const s_sym * sym_ht_find (s_sym_ht *ht, const s_str *str)
{
  uw h;
  s_sym_ht_item *item;
  uw pos;
  h = primehash_uw_inline(str, 0);
#if HAVE_PTHREAD
  rwlock_r(&ht->rwlock);
#endif
  pos = h & SYM_HT_MASK(ht);
  item = ht->item[pos];
  while (item && ((item->hash_uw != h) ||
                  compare_str(str, &item->sym->str) != 0))
    item = item->next;
  if (item) {
#if HAVE_PTHREAD
    rwlock_unlock_r(&ht->rwlock);
#endif
    return item->sym;
  }
#if HAVE_PTHREAD
  rwlock_unlock_r(&ht->rwlock);
#endif
  return NULL;
}

s_sym_ht * sym_ht_new (u8 size_exp)
{
  s_sym_ht *ht;
  if (! size_exp)
    size_exp = SYM_HT_SIZE_EXP_DEFAULT;
  if (! (ht = alloc(sizeof(s_sym_ht))))
    return NULL;
  ht->size_exp = size_exp;
  ht->item = alloc((1 << size_exp) * sizeof(s_sym_ht_item *));
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

const s_sym * sym_ht_register (s_sym_ht *ht, const s_sym *sym,
                               s_sym *sym_free)
{
  bool collision;
  uw h;
  s_sym_ht_item *i;
  s_sym_ht_item **item;
  uw pos;
  h = primehash_uw_inline(&sym->str, 0);
#if HAVE_PTHREAD
  rwlock_w(&ht->rwlock);
#endif
  pos = h & ((1 << ht->size_exp) - 1);
  item = ht->item + pos;
  collision = item ? true : false;
  if ((i = *item)) {
    while (i) {
      if (i->hash_uw == h && compare_str(&i->sym->str, &sym->str) == 0) {
        fprintf(stderr, "sym_ht_register: %s == %s\n",
                sym->str.ptr.p_pchar,
                i->sym->str.ptr.p_pchar);
        abort();
      }
      i = i->next;
    }
  }
  if (! (i = sym_ht_item_new(h, sym, sym_free, *item))) {
#if HAVE_PTHREAD
    rwlock_unlock_w(&ht->rwlock);
#endif
    return NULL;
  }
  *item = i;
  ht->count++;
  if (collision)
    ht->collisions++;
#if HAVE_PTHREAD
  rwlock_unlock_w(&ht->rwlock);
#endif
  return sym;
}
