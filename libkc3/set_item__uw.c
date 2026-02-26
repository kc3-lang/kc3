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
/* Gen from set_item.c.in NAME=uw TYPE=uw */
#include "alloc.h"
#include "assert.h"
#include "set_item__uw.h"
#include "uw.h"

s_set_item__uw *
set_item_new__uw (uw data, uw hash, s_set_item__uw *next)
{
  s_set_item__uw *item;
  item = alloc(sizeof(s_set_item__uw));
  if (! item)
    return NULL;
  if (! uw_init_copy(&item->data, data)) {
    alloc_free(item);
    return NULL;
  }
  item->hash = hash;
  item->next = next;
  item->usage = 0;
  return item;
}

void set_item_delete__uw (s_set_item__uw *x)
{
  assert(x);
  uw_clean(&x->data);
  alloc_free(x);
}

void set_item_delete_all__uw (s_set_item__uw *x)
{
  s_set_item__uw *y;
  while (x) {
    y = x;
    x = x->next;
    set_item_delete__uw(y);
  }
}
