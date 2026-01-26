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
/* Gen from set_cursor.c.in NAME=fact TYPE=s_fact * */
#include <assert.h>
#include <stdlib.h>
#include "types.h"

void
set_cursor_init__fact (s_set__fact *set, s_set_cursor__fact *c)
{
  assert(set);
  assert(c);
  c->set = set;
  c->i = 0;
  c->item = NULL;
  c->count = 0;
}

static s_set_item__fact *
set_cursor_next_index__fact (s_set_cursor__fact *c)
{
  assert(c);
  assert(c->set);
  while (c->i < c->set->max &&
         ! c->item) {
    c->item = c->set->items[c->i];
    c->i++;
  }
  if (c->item) {
    c->count++;
    return c->item;
  }
  return NULL;
}

s_set_item__fact *
set_cursor_next__fact (s_set_cursor__fact *c)
{
  assert(c);
  if (c->count == c->set->count)
    return NULL;
  if (c->item)
    c->item = c->item->next;
  if (!c->item)
    return set_cursor_next_index__fact(c);
  c->count++;
  return c->item;
}
