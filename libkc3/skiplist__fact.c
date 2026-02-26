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
#include "fact.h"
#include "skiplist_node__fact.h"
#include "skiplist__fact.h"
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
skiplist_clean__fact
(s_skiplist__fact *skiplist)
{
  s_skiplist_node__fact *node;
  assert(skiplist);
  assert(skiplist->head);
  while ((node = SKIPLIST_NODE_NEXT__fact(skiplist->head, 0)))
    skiplist_remove__fact(skiplist, node->fact);
  skiplist_node_delete__fact(skiplist->head);
}

void
skiplist_delete__fact
(s_skiplist__fact *skiplist)
{
  assert(skiplist);
  skiplist_clean__fact(skiplist);
  alloc_free(skiplist);
}

s_skiplist_node__fact *
skiplist_find__fact
(s_skiplist__fact *skiplist,
 const s_fact * fact)
{
  s_skiplist_node__fact *node = skiplist->head;
  u8 level = node->height;
  while (level--) {
    s_skiplist_node__fact *n = node;
    s8 c = -1;
    while (n && (c = skiplist->compare(fact, n->fact)) > 0)
      n = SKIPLIST_NODE_NEXT__fact(n, level);
    if (c == 0)
      return n;
  }
  return NULL;
}

static s_skiplist__fact *
skiplist_height_table_init__fact
(s_skiplist__fact *skiplist,
 f64 spacing)
{
  t_skiplist_height *height_table;
  u8 h;
  f64 w = spacing;
  f64 end = w;
  height_table = SKIPLIST_HEIGHT_TABLE__fact(skiplist);
  for (h = 0; h < skiplist->max_height; h++) {
    w *= spacing;
    end += w;
    if (end > (f64) SKIPLIST_HEIGHT_MAX)
      return NULL;
    height_table[h] = end;
  }
  return skiplist;
}

s_skiplist__fact *
skiplist_init__fact
(s_skiplist__fact *skiplist,
 u8 max_height,
 f64 spacing)
{
  assert(skiplist);
  skiplist->max_height = max_height;
  skiplist_height_table_init__fact(skiplist, spacing);
  skiplist->head = skiplist_node_new__fact(NULL, max_height);
  if (! skiplist->head)
    return NULL;
  skiplist->compare = compare_fact;
  skiplist->length = 0;
  return skiplist;
}

s_skiplist_node__fact *
skiplist_insert__fact
(s_skiplist__fact *skiplist,
 s_fact * fact)
{
  s_skiplist_node__fact *pred;
  s_skiplist_node__fact *next;
  uw height;
  s_skiplist_node__fact *node;
  pred = skiplist_pred__fact(skiplist, fact);
  next = SKIPLIST_NODE_NEXT__fact(pred, 0);
  next = SKIPLIST_NODE_NEXT__fact(next, 0);
  if (next && skiplist->compare(fact, next->fact) == 0)
    return next;
  height = skiplist_random_height__fact(skiplist);
  node = skiplist_node_new__fact(fact, height);
  skiplist_node_insert__fact(node, pred);
  skiplist->length++;
  skiplist_node_delete__fact(pred);
  return node;
}

s_skiplist__fact *
skiplist_new__fact
(u8 max_height,
 f64 spacing)
{
  s_skiplist__fact *skiplist;
  skiplist = alloc(SKIPLIST_SIZE__fact(max_height));
  if (! skiplist)
    return NULL;
  if (! skiplist_init__fact(skiplist, max_height, spacing)) {
    alloc_free(skiplist);
    return NULL;
  }
  return skiplist;
}

s_skiplist_node__fact *
skiplist_pred__fact
(s_skiplist__fact *skiplist,
 const s_fact * fact)
{
  int level;
  s_skiplist_node__fact *pred;
  s_skiplist_node__fact *n;
  s_skiplist_node__fact *node;
  assert(skiplist);
  level = skiplist->max_height;
  pred = skiplist_node_new__fact(NULL, skiplist->max_height);
  if (! pred)
    return NULL;
  node = skiplist->head;
  while (level--) {
    n = node;
    while (n && skiplist->compare(fact, n->fact) > 0) {
      node = n;
      n = SKIPLIST_NODE_NEXT__fact(node, level);
    }
    SKIPLIST_NODE_NEXT__fact(pred, level) = node;
  }
  return pred;
}

u8
skiplist_random_height__fact
(s_skiplist__fact *skiplist)
{
  u16 height;
  const t_skiplist_height *height_table;
  sw i;
  u64 j;
  sw max;
  t_skiplist_height k;
  assert(skiplist);
  assert(skiplist->max_height);
  height_table = SKIPLIST_HEIGHT_TABLE__fact(skiplist);
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
skiplist_remove__fact
(s_skiplist__fact *skiplist,
 const s_fact * fact)
{
  uw level;
  s_skiplist_node__fact *pred;
  s_skiplist_node__fact *next;
  pred = skiplist_pred__fact(skiplist, fact);
  assert(pred);
  next = SKIPLIST_NODE_NEXT__fact(pred, 0);
  assert(next);
  next = SKIPLIST_NODE_NEXT__fact(next, 0);
  if (!next || skiplist->compare(fact, next->fact) != 0) {
    skiplist_node_delete__fact(pred);
    return false;
  }
  for (level = 0; level < next->height; level++) {
    s_skiplist_node__fact *p =
      SKIPLIST_NODE_NEXT__fact(pred, level);
    SKIPLIST_NODE_NEXT__fact(p, level) =
      SKIPLIST_NODE_NEXT__fact(next, level);
  }
  skiplist->length--;
  skiplist_node_delete__fact(pred);
  skiplist_node_delete__fact(next);
  return true;
}

void
skiplist_remove_all__fact
(s_skiplist__fact *skiplist)
{
  s_skiplist_node__fact *node;
  while ((node = SKIPLIST_NODE_NEXT__fact(skiplist->head, 0)))
    skiplist_remove__fact(skiplist, node->fact);
}
