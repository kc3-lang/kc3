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
#include "facts_spec.h"
#include "facts_spec_cursor.h"
#include "list.h"
#include "tag.h"

uw facts_spec_count_facts (p_facts_spec spec)
{
  s_facts_spec_cursor cursor;
  uw count = 0;
  s_fact fact;
  facts_spec_cursor_init(&cursor, spec);
  while (facts_spec_cursor_next(&cursor, &fact))
    count++;
  return count;
}

p_facts_spec facts_spec_new_expand (p_facts_spec spec)
{
  uw count;
  s_facts_spec_cursor cursor;
  s_fact fact;
  p_facts_spec new;
  p_facts_spec n;
  assert(spec);
  count = facts_spec_count_facts(spec);
  if (count > 0) {
    new = alloc((count * 4 + 1) * sizeof(s_tag *));
    if (! new)
      return NULL;
    n = new;
    facts_spec_cursor_init(&cursor, spec);
    while (facts_spec_cursor_next(&cursor, &fact)) {
      *n = (s_tag *) fact.subject;
      while ((*n)->type == TAG_PVAR &&
             (*n)->data.pvar->bound)
        *n = &(*n)->data.pvar->tag;
      n++;
      *n = (s_tag *) fact.predicate;
      while ((*n)->type == TAG_PVAR &&
             (*n)->data.pvar->bound)
        *n = &(*n)->data.pvar->tag;
      n++;
      *n = (s_tag *) fact.object;
      while ((*n)->type == TAG_PVAR &&
             (*n)->data.pvar->bound)
        *n = &(*n)->data.pvar->tag;
      n++;
      *n++ = NULL;
    }
    *n = NULL;
    return new;
  }
  return NULL;
}

p_facts_spec facts_spec_new_list (s_list *spec)
{
  uw c;
  uw count = 1;
  p_facts_spec new;
  p_facts_spec n;
  s_list *s;
  s_list *t;
  assert(spec);
  if (! spec)
    return NULL;
  s = spec;
  while (s) {
    if (s->tag.type != TAG_PLIST) {
      err_puts("facts_spec_new_list: invalid spec: not a List of List");
      assert(! "facts_spec_new_list: invalid spec: not a List of List");
      return NULL;
    }
    t = s->tag.data.plist;
    if ((c = list_length(t)) < 3) {
      err_puts("facts_spec_new_list: invalid spec: list length < 3");
      assert(! "facts_spec_new_list: invalid spec: list length < 3");
      return NULL;
    }
    if ((c - 1) % 2) {
      err_puts("facts_spec_new_list: invalid spec: list length"
               " != 3 + 2 * n");
      assert(! "facts_spec_new_list: invalid spec: list length"
               " != 3 + 2 * n");
      return NULL;
    }
    count += c + 1;
    s = list_next(s);
  }
  new = alloc(count * sizeof(s_tag *));
  if (! new)
    return NULL;
  n = new;
  s = spec;
  while (s) {
    t = s->tag.data.plist;
    while (t) {
      *n++ = &t->tag;
      t = list_next(t);
    }
    *n++ = NULL;
    s = list_next(s);
  }
  *n = NULL;
  return new;
}

p_facts_spec facts_spec_sort (p_facts_spec spec)
{
  s_tag **a;
  s_tag **b;
  uw count;
  uw i = 0;
  uw j;
  s8 r;
  assert(spec);
  count = facts_spec_count_facts(spec);
  if (count > 0)
    while (i < count - 1) {
      j = 0;
      while (j < count - i - 1) {
        a = spec + j * 4;
        b = spec + (j + 1) * 4;
        r = compare_fact_unbound_var_count((s_fact *) a,
                                           (s_fact *) b);
        if (r == COMPARE_ERROR)
          return NULL;
        if (r > 0) {
          s_tag *swap[3];
          swap[0] = a[0];
          swap[1] = a[1];
          swap[2] = a[2];
          a[0] = b[0];
          a[1] = b[1];
          a[2] = b[2];
          b[0] = swap[0];
          b[1] = swap[1];
          b[2] = swap[2];
        }
        j++;
      }
      i++;
    }
  return spec;
}
