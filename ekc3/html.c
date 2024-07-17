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
#include "html.h"

s_str * html_escape (const s_str *str, s_str *dest)
{
  s_buf buf;
  const s_list *e;
  const s_list *escape;
  s_ident       escape_ident;
  s_tag         escape_tag;
  s_tag *replace;
  s_tag *reserved;
  s_str s;
  s_tag tag_c;
  assert(str);
  assert(dest);
  ident_init(&escape_ident, sym_1("EKC3"), sym_1("html_escape"));
  if (! ident_get(&escape_ident, &escape_tag)) {
    err_puts("html_escape: ident_get");
    assert(! "html_escape: ident_get");
    return NULL;
  }
  if (! tag_is_an_associative_list(&escape_tag)) {
    err_puts("html_escape: EKC3.html_escape is not an associative"
             " List");
    assert(!("html_escape: EKC3.html_escape is not an associative"
             " List"));
    return NULL;
  }
  escape = escape_tag->data.list;
  if (! buf_init_alloc(&buf, str->size * 8))
    return NULL;
  tag_c.type = TAG_CHARACTER;
  s = *str;
  while (str_read_character_utf8(&s, &tag_c.data.character) > 0) {
    replace = NULL;
    e = escape;
    while (e) {
      reserved = e->tag.data.tuple.tag;
      if (reserved->type != TAG_STR) {
        err_puts("html_escape: EKC3.html_escape: reserved that is"
                 " not a Str");
        assert(!("html_escape: EKC3.html_escape: reserved that is"
                 " not a Str"));
        goto ko;
      }
      if (! compare_tag(reserved, &tag_c)) {
        replace = e->tag.data.tuple.tag + 1;
        if (replace->type != TAG_STR) {
          err_puts("html_escape: EKC3.html_escape: replacement that is"
                   " not a Str");
          assert(!("html_escape: EKC3.html_escape: replacement that is"
                   " not a Str"));
          goto ko;
        }
        break;
      }
      e = list_next(e);
    }
    if (replace) {
      if (buf_write_str(&buf, &replace->data.str) <= 0) {
        err_puts("html_escape: buf_write_str");
        assert(!("html_escape: buf_write_str");
        goto ko;
      }
    }
    else {
      if (buf_write_character_utf8(&buf, &tag_c.data.character) <= 0) {
        err_puts("html_escape: buf_write_character_utf8");
        assert(!("html_escape: buf_write_character_utf8");
        goto ko;
      }
    }
  }
  s = (s_str) {0};
  if (! buf_read_to_str(&buf, &s)) {
    buf_clean(&buf);
    return NULL;
  }
  *dest = s;
  return dest;
 ko:
  buf_clean(&buf);
  return NULL;
}
