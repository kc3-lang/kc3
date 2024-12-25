/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "alloc.h"
#include "skiplist_node__alloc.h"
#include "skiplist__alloc.h"
#include "u32.h"
#include "u64.h"

/*
  Random height
  -------------

  ∀ U ∈ ℕ : 1 < U
  ∀ n ∈ ℕ*
  ∀ r ∈ ℕ : r ≤ n
  ∀ random : ℕ* ⟶ ℕ
             random(n) ∈ [0..n-1]
             ∀ i ∈ [0..n-1], P(random(n) = i) = n⁻¹               (i)
  Qᵣ := random(Uⁿ) < Uⁿ⁻ʳ

  (i) ⇒        P(random(n) < v) = ∑ᵢ₌₀ᵛ⁻¹ P(random(n) = i)
      ⇒        P(random(n) < v) = v . n⁻¹                        (ii)

      ⇒    P(random(Uⁿ) < Uⁿ⁻ʳ) = Uⁿ⁻ʳ . (Uⁿ)⁻¹
      ⇔                   P(Qᵣ) = U⁻ʳ                           (iii)

  P(Qₙ) = P(random(Uⁿ) < U⁰)
        = P(random(Uⁿ) < 1)
        = P(random(Uⁿ) = 0)
        = U⁻ⁿ

  R := maxᵣ(Qᵣ)
     = maxᵣ(random(Uⁿ) < Uⁿ⁻ʳ)
     = maxᵣ(random(Uⁿ) + 1 ≤ Uⁿ⁻ʳ)
     = maxᵣ(logᵤ(random(Uⁿ) + 1) ≤ n - r)
     = maxᵣ(⌈logᵤ(random(Uⁿ) + 1)⌉ ≤ n - r)
     = maxᵣ(r ≤ n - ⌈logᵤ(random(Uⁿ) + 1)⌉)
     = n - ⌈logᵤ(random(Uⁿ) + 1)⌉                                (iv)

                       0 ≤ random(Uⁿ) < Uⁿ
   ⇔                   1 ≤ random(Uⁿ)+1 ≤ Uⁿ
   ⇔        logᵤ(1) ≤ logᵤ(random(Uⁿ)+1) ≤ logᵤ(Uⁿ)
   ⇔             0 ≤ ⌈logᵤ(random(Uⁿ)+1)⌉ ≤ n
   ⇔           -n ≤ -⌈logᵤ(random(Uⁿ)+1)⌉ ≤ 0
   ⇔         0 ≤ n - ⌈logᵤ(random(Uⁿ)+1)⌉ ≤ n
   ⇔                      0 ≤ R ≤ n                               (v)
*/

void
skiplist_clean__alloc (s_skiplist__alloc *skiplist)
{
  s_skiplist_node__alloc *node;
  assert(skiplist);
  assert(skiplist->head);
  while ((node = SKIPLIST_NODE_NEXT__alloc(skiplist->head, 0)))
    skiplist_remove__alloc(skiplist, node->alloc);
  skiplist_node_delete__alloc(skiplist->head);
}

void
skiplist_delete__alloc (s_skiplist__alloc *skiplist)
{
  assert(skiplist);
  skiplist_clean__alloc(skiplist);
  free(skiplist);
}

s_skiplist_node__alloc *
skiplist_find__alloc (s_skiplist__alloc *skiplist, const s_alloc * alloc)
{
  s_skiplist_node__alloc *node = skiplist->head;
  u8 level = node->height;
  while (level--) {
    s_skiplist_node__alloc *n = node;
    s8 c = -1;
    while (n && (c = skiplist->compare(alloc, n->alloc)) > 0)
      n = SKIPLIST_NODE_NEXT__alloc(n, level);
    if (c == 0)
      return n;
  }
  return NULL;
}

static s_skiplist__alloc *
skiplist_height_table_init__alloc (s_skiplist__alloc *skiplist, f64 spacing)
{
  t_skiplist_height *height_table;
  u8 h;
  f128 w = spacing;
  f128 end = w;
  height_table = SKIPLIST_HEIGHT_TABLE__alloc(skiplist);
  for (h = 0; h < skiplist->max_height; h++) {
    w *= spacing;
    end += w;
    if (end > (f128) SKIPLIST_HEIGHT_MAX)
      return NULL;
    height_table[h] = end;
  }
  return skiplist;
}

s_skiplist__alloc *
skiplist_init__alloc (s_skiplist__alloc *skiplist, u8 max_height, f64 spacing)
{
  assert(skiplist);
  skiplist->max_height = max_height;
  skiplist_height_table_init__alloc(skiplist, spacing);
  skiplist->head = skiplist_node_new__alloc(NULL, max_height);
  if (! skiplist->head)
    return NULL;
  skiplist->compare = compare_alloc;
  skiplist->length = 0;
  return skiplist;
}

s_skiplist_node__alloc *
skiplist_insert__alloc (s_skiplist__alloc *skiplist, s_alloc * alloc)
{
  s_skiplist_node__alloc *pred;
  s_skiplist_node__alloc *next;
  uw height;
  s_skiplist_node__alloc *node;
  pred = skiplist_pred__alloc(skiplist, alloc);
  next = SKIPLIST_NODE_NEXT__alloc(pred, 0);
  next = SKIPLIST_NODE_NEXT__alloc(next, 0);
  if (next && skiplist->compare(alloc, next->alloc) == 0)
    return next;
  height = skiplist_random_height__alloc(skiplist);
  node = skiplist_node_new__alloc(alloc, height);
  skiplist_node_insert__alloc(node, pred);
  skiplist->length++;
  skiplist_node_delete__alloc(pred);
  return node;
}

s_skiplist__alloc *
skiplist_new__alloc (u8 max_height, f64 spacing)
{
  s_skiplist__alloc *skiplist;
  skiplist = alloc(SKIPLIST_SIZE__alloc(max_height));
  if (! skiplist)
    return NULL;
  if (! skiplist_init__alloc(skiplist, max_height, spacing)) {
    free(skiplist);
    return NULL;
  }
  return skiplist;
}

s_skiplist_node__alloc *
skiplist_pred__alloc (s_skiplist__alloc *skiplist, const s_alloc * alloc)
{
  int level;
  s_skiplist_node__alloc *pred;
  s_skiplist_node__alloc *n;
  s_skiplist_node__alloc *node;
  assert(skiplist);
  level = skiplist->max_height;
  pred = skiplist_node_new__alloc(NULL, skiplist->max_height);
  if (! pred)
    return NULL;
  node = skiplist->head;
  while (level--) {
    n = node;
    while (n && skiplist->compare(alloc, n->alloc) > 0) {
      node = n;
      n = SKIPLIST_NODE_NEXT__alloc(node, level);
    }
    SKIPLIST_NODE_NEXT__alloc(pred, level) = node;
  }
  return pred;
}

u8
skiplist_random_height__alloc (s_skiplist__alloc *skiplist)
{
  u16 height;
  const t_skiplist_height *height_table;
  sw i;
  u64 j;
  sw max;
  t_skiplist_height k;
  assert(skiplist);
  assert(skiplist->max_height);
  height_table = SKIPLIST_HEIGHT_TABLE__alloc(skiplist);
  max = height_table[skiplist->max_height - 1];
  u64_random_uniform(&j, max);
  k = j;
  i = 0;
  while (i < skiplist->max_height - 1 &&
         k > height_table[i])
    i++;
  height = skiplist->max_height - i;
  assert(height);
  return height;
}

bool
skiplist_remove__alloc (s_skiplist__alloc *skiplist, const s_alloc * alloc)
{
  uw level;
  s_skiplist_node__alloc *pred;
  s_skiplist_node__alloc *next;
  pred = skiplist_pred__alloc(skiplist, alloc);
  assert(pred);
  next = SKIPLIST_NODE_NEXT__alloc(pred, 0);
  assert(next);
  next = SKIPLIST_NODE_NEXT__alloc(next, 0);
  if (!next || skiplist->compare(alloc, next->alloc) != 0) {
    skiplist_node_delete__alloc(pred);
    return false;
  }
  for (level = 0; level < next->height; level++) {
    s_skiplist_node__alloc *p =
      SKIPLIST_NODE_NEXT__alloc(pred, level);
    SKIPLIST_NODE_NEXT__alloc(p, level) =
      SKIPLIST_NODE_NEXT__alloc(next, level);
  }
  skiplist->length--;
  skiplist_node_delete__alloc(pred);
  skiplist_node_delete__alloc(next);
  return true;
}
