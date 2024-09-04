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
#include <md4c.h>
#include <md4c-html.h>
#include <libkc3/kc3.h>
#include "markdown.h"

void markdown_to_html_str_callback (const MD_CHAR *, MD_SIZE, void *);

s_str * markdown_to_html_str (const s_str *markdown, s_str *dest)
{
  s_list *list;
  s_list **l;
  s_str tmp;
  assert(markdown);
  assert(dest);
  list = NULL;
  l = &list;
  md_html(markdown->ptr.pchar, markdown->size,
          markdown_to_html_str_callback,
          &l, 0, 0);
  if (! str_init_concatenate_list(&tmp,
                                  (const s_list * const *) &list)) {
    list_delete_all(list);
    return NULL;
  }
  list_delete_all(list);
  *dest = tmp;
  return dest;
}

void markdown_to_html_str_callback (const MD_CHAR *p, MD_SIZE size,
                                    void *data)
{
  s_list **l;
  l = *(s_list ***) data;
  if (! (*l = list_new_str_alloc_copy(size, p, NULL))) {
    err_puts("markdown_to_html_str_callback: list_new_str_cpy");
    assert(! "markdown_to_html_str_callback: list_new_str_cpy");
    return;
  }
  l = &(*l)->next.data.list;
  *(s_list ***) data = l;
}
