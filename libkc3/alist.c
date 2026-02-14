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
#include "kc3_main.h"
#include "list.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"

s_tag * alist_access (s_list *alist,
                      s_list *key,
                      s_tag *dest)
{
  s_tag *first;
  s_list *next;
  s_tag *r;
  s_tag tag;
  assert(key);
  assert(dest);
  if (! list_is_alist(alist)) {
    err_puts("alist_access: not an associative list");
    assert(! "alist_access: not an associative list");
    return NULL;
  }
  first = &key->tag;
  next = list_next(key);
  if (! next)
    return alist_get(alist, first, dest);
  if (! alist_get(alist, first, &tag)) {
    err_write_1("alist_access: alist_get(");
    err_inspect_list(alist);
    err_write_1(", ");
    err_inspect_tag(first);
    err_write_1(")\n");
    assert(! "alist_access: alist_get");
    return NULL;
  }
  r = kc3_access(&tag, &next, dest);
  tag_clean(&tag);
  return r;
}

s_tag * alist_get (s_list *alist, s_tag *key, s_tag *dest)
{
  s_list *l;
  assert(list_is_alist(alist));
  assert(key);
  assert(dest);
  l = alist;
  while (l) {
    if (! compare_tag(&l->tag.data.ptuple->tag[0], key))
      return tag_init_copy(dest, l->tag.data.ptuple->tag + 1);
    l = list_next(l);
  }
  return NULL;
}
