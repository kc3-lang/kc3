/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
/**
 * @file to_lisp.h
 * @brief Convert Tag to Lisp notation
 *
 * Convert any Tag to Lisp notation, that is List for code.
 */
#include "assert.h"
#include "list.h"
#include "tag.h"
#include "to_lisp.h"

s_tag * to_lisp (s_tag *tag, s_tag *dest)
{
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_CALL:
    return to_lisp_call(&tag->data.call, dest);
  case TAG_LIST:
    return to_lisp_list(tag->data.list, dest);
  case TAG_TUPLE:
    return to_lisp_tuple(&tag->data.tuple, dest);
  default:
    return tag_init_copy(dest, tag);
  }
}

s_tag * to_lisp_call (s_call *call, s_tag *dest)
{
  s_tag arguments;
  s_list *list;
  if (! to_lisp_list(call->arguments, &arguments))
    return NULL;
  if (arguments.type != TAG_LIST) {
    err_puts("to_lisp_call: arguments.type != TAG_LIST");
    assert(! "to_lisp_call: arguments.type != TAG_LIST");
    return NULL;
  }
  if (! (list = list_new_ident(&call->ident, arguments.data.list))) {
    tag_clean(&arguments);
    return NULL;
  }
  return tag_init_list(dest, list);
}

s_tag * to_lisp_list (s_list *list, s_tag *dest)
{
  s_list *list_i;
  s_list **tail;
  s_list *tmp;
  tmp = NULL;
  tail = &tmp;
  list_i = list;
  while (list_i) {
    *tail = list_new(NULL);
    if (! to_lisp(&list_i->tag, &(*tail)->tag))
      goto ko;
    tail = &(*tail)->next.data.list;
    list_i = list_next(list_i);
  }
  return tag_init_list(dest, tmp);
 ko:
  list_delete_all(tmp);
  return NULL;
}

s_tag * to_lisp_tuple (s_tuple *tuple, s_tag *dest)
{
  uw i;
  s_tag tmp = {0};
  if (! tag_init_tuple(&tmp, tuple->count))
    return NULL;
  i = 0;
  while (i < tuple->count) {
    if (! to_lisp(tuple->tag + i, tmp.data.tuple.tag + i))
      goto ko;
    i++;
  }
  *dest = tmp;
  return dest;
 ko:
  tag_clean(&tmp);
  return NULL;
}
