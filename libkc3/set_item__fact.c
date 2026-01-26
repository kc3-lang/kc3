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
/* Gen from set_item.c.in NAME=fact TYPE=s_fact * */
#include "alloc.h"
#include "assert.h"
#include "set_item__fact.h"
#include "fact.h"

s_set_item__fact *
set_item_new__fact (s_fact * data, uw hash, s_set_item__fact *next)
{
  s_set_item__fact *item;
  item = alloc(sizeof(s_set_item__fact));
  if (! item)
    return NULL;
  if (! fact_init_copy(&item->data, data)) {
    free(item);
    return NULL;
  }
  item->hash = hash;
  item->next = next;
  item->usage = 0;
  return item;
}

void set_item_delete__fact (s_set_item__fact *x)
{
  assert(x);
  fact_clean(&x->data);
  free(x);
}

void set_item_delete_all__fact (s_set_item__fact *x)
{
  s_set_item__fact *y;
  while (x) {
    y = x;
    x = x->next;
    set_item_delete__fact(y);
  }
}
