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
  case TAG_PCALL:
    return to_lisp_call(tag->data.td_pcall, dest);
  case TAG_PLIST:
    return to_lisp_list(tag->data.td_plist, dest);
  case TAG_PTUPLE:
    return to_lisp_tuple(tag->data.td_ptuple, dest);
  default:
    return tag_init_copy(dest, tag);
  }
}

s_tag * to_lisp_call (s_call *call, s_tag *dest)
{
  s_tag arguments;
  p_list list;
  if (! to_lisp_list(call->arguments, &arguments))
    return NULL;
  if (arguments.type != TAG_PLIST) {
    err_puts("to_lisp_call: arguments.type != TAG_PLIST");
    assert(! "to_lisp_call: arguments.type != TAG_PLIST");
    return NULL;
  }
  if (! (list = list_new_ident(&call->ident, arguments.data.td_plist))) {
    tag_clean(&arguments);
    return NULL;
  }
  return tag_init_plist(dest, list);
}

s_tag * to_lisp_list (p_list list, s_tag *dest)
{
  p_list list_i;
  p_list *tail;
  p_list tmp;
  tmp = NULL;
  tail = &tmp;
  list_i = list;
  while (list_i) {
    *tail = list_new(NULL);
    if (! to_lisp(&list_i->tag, &(*tail)->tag))
      goto ko;
    tail = &(*tail)->next.data.td_plist;
    list_i = list_next(list_i);
  }
  return tag_init_plist(dest, tmp);
 ko:
  list_delete_all(tmp);
  return NULL;
}

s_tag * to_lisp_tuple (s_tuple *tuple, s_tag *dest)
{
  uw i;
  s_tag tmp = {0};
  if (! tag_init_ptuple(&tmp, tuple->count))
    return NULL;
  i = 0;
  while (i < tuple->count) {
    if (! to_lisp(tuple->tag + i, tmp.data.td_ptuple->tag + i))
      goto ko;
    i++;
  }
  *dest = tmp;
  return dest;
 ko:
  tag_clean(&tmp);
  return NULL;
}
