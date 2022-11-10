/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include "fact.h"
#include "facts_spec.h"
#include "facts_spec_cursor.h"
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
  assert(spec);
  count = facts_spec_count_facts(spec);
  if (count > 0) {
    s_facts_spec_cursor cursor;
    s_fact fact;
    p_facts_spec new = calloc(count * 4 + 1, sizeof(s_tag *));
    p_facts_spec n = new;
    facts_spec_cursor_init(&cursor, spec);
    while (facts_spec_cursor_next(&cursor, &fact)) {
      *n++ = (s_tag *) fact.subject;
      *n++ = (s_tag *) fact.predicate;
      *n++ = (s_tag *) fact.object;
      *n++ = NULL;
    }
    *n = NULL;
    return new;
  }
  return NULL;
}

p_facts_spec facts_spec_sort (p_facts_spec spec)
{
  s_tag **a;
  s_tag **b;
  uw count;
  uw i = 0;
  uw j;
  assert(spec);
  count = facts_spec_count_facts(spec);
  if (count > 0)
    while (i < count - 1) {
      j = 0;
      while (j < count - i - 1) {
        a = spec + j * 4;
        b = spec + (j + 1) * 4;
        if (fact_compare_var_count((s_fact *) a, (s_fact *) b) > 0) {
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
