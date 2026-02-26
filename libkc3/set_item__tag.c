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
/* Gen from set_item.c.in NAME=tag TYPE=s_tag * */
#include "alloc.h"
#include "assert.h"
#include "set_item__tag.h"
#include "tag.h"

s_set_item__tag *
set_item_new__tag (s_tag * data, uw hash, s_set_item__tag *next)
{
  s_set_item__tag *item;
  item = alloc(sizeof(s_set_item__tag));
  if (! item)
    return NULL;
  if (! tag_init_copy(&item->data, data)) {
    alloc_free(item);
    return NULL;
  }
  item->hash = hash;
  item->next = next;
  item->usage = 0;
  return item;
}

void set_item_delete__tag (s_set_item__tag *x)
{
  assert(x);
  tag_clean(&x->data);
  alloc_free(x);
}

void set_item_delete_all__tag (s_set_item__tag *x)
{
  s_set_item__tag *y;
  while (x) {
    y = x;
    x = x->next;
    set_item_delete__tag(y);
  }
}
