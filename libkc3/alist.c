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
#include <string.h>
#include "alist.h"
#include "alloc.h"
#include "assert.h"
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "compare.h"
#include "data.h"
#include "list.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"

s_tag * alist_get (const s_list * const *alist, const s_tag *key,
                   s_tag *dest)
{
  const s_list *l;
  assert(alist);
  assert(list_is_alist(alist));
  assert(key);
  assert(dest);
  l = *alist;
  while (l) {
    if (! compare_tag(&l->tag.data.tuple.tag[0], key))
      return tag_init_copy(dest, l->tag.data.tuple.tag + 1);
    l = list_next(l);
  }
  return NULL;
}
