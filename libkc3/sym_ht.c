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

#define SYM_HT_SIZE(ht) ((uw) 1 << ht->size_exp)

#define SYM_HT_SIZE_EXP_DEFAULT 20

#define SYM_HT_MASK(ht) (SYM_HT_SIZE(ht) - 1)

static s_sym_ht_item * sym_ht_find_item (s_sym_ht *ht,
                                         const s_str *str, uw hash)
{
  s_sym_ht_item *item;
  item = ht->item[hash & SYM_HT_MASK(ht)];
  while (item && ((item->hash_uw != hash) ||
                  compare_str(str, &item->sym->str) != 0))
    item = item->next;
  return item;
}

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
  h = primehash_uw_inline(str, 0);
#if HAVE_PTHREAD
  rwlock_r(&ht->rwlock);
#endif
  item = sym_ht_find_item(ht, str, h);
#if HAVE_PTHREAD
  rwlock_unlock_r(&ht->rwlock);
#endif
  if (item)
    return item->sym;
  return NULL;
}

s_sym_ht * sym_ht_new (u8 size_exp)
{
  s_sym_ht *ht;
  uw size;
  if (! size_exp)
    size_exp = SYM_HT_SIZE_EXP_DEFAULT;
  if (size_exp >= sizeof(uw) * 8)
    return NULL;
  size = (uw) 1 << size_exp;
  if (size > UW_MAX / sizeof(s_sym_ht_item *))
    return NULL;
  if (! (ht = alloc(sizeof(s_sym_ht))))
    return NULL;
  ht->size_exp = size_exp;
  if (! (ht->item = alloc(size * sizeof(s_sym_ht_item *)))) {
    alloc_free(ht);
    return NULL;
  }
#if HAVE_PTHREAD
  if (! rwlock_init(&ht->rwlock)) {
    alloc_free(ht->item);
    alloc_free(ht);
    return NULL;
  }
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
  pos = h & SYM_HT_MASK(ht);
  item = ht->item + pos;
  collision = *item ? true : false;
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

const s_sym * sym_ht_intern (s_sym_ht *ht, const s_str *str)
{
  s_sym_ht_item *candidate;
  s_sym *candidate_sym;
  bool collision;
  s_sym_ht_item *found;
  uw h;
  s_sym_ht_item **item;
  uw pos;
  h = primehash_uw_inline(str, 0);
#if HAVE_PTHREAD
  rwlock_r(&ht->rwlock);
#endif
  found = sym_ht_find_item(ht, str, h);
#if HAVE_PTHREAD
  rwlock_unlock_r(&ht->rwlock);
#endif
  if (found)
    return found->sym;
  if (! (candidate_sym = alloc(sizeof(s_sym))))
    return NULL;
  if (! str_init_copy(&candidate_sym->str, str)) {
    alloc_free(candidate_sym);
    return NULL;
  }
  if (! (candidate = sym_ht_item_new(h, candidate_sym, candidate_sym,
                                     NULL))) {
    sym_delete(candidate_sym);
    return NULL;
  }
#if HAVE_PTHREAD
  rwlock_w(&ht->rwlock);
#endif
  found = sym_ht_find_item(ht, str, h);
  if (found) {
#if HAVE_PTHREAD
    rwlock_unlock_w(&ht->rwlock);
#endif
    sym_ht_item_delete(candidate);
    return found->sym;
  }
  pos = h & SYM_HT_MASK(ht);
  item = ht->item + pos;
  collision = *item ? true : false;
  candidate->next = *item;
  *item = candidate;
  ht->count++;
  if (collision)
    ht->collisions++;
#if HAVE_PTHREAD
  rwlock_unlock_w(&ht->rwlock);
#endif
  return candidate_sym;
}
