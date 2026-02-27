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
#include "../libkc3/kc3.h"
#include "html.h"

s_str * html_escape (s_tag *src, s_str *dest)
{
  s_buf buf;
  character c;
  const s_list *e;
  const s_list *escape;
  s_ident       escape_ident;
  s_tag         escape_tag;
  s_tag *replace;
  s_tag *reserved;
  s_str s;
  s_str str = {0};
  p_sym sym_Str = &g_sym_Str;
  s_tag tag;
  assert(src);
  assert(dest);
  if (! str_init_cast(&str, &sym_Str, src)) {
    err_puts("html_escape: cannot cast to Str");
    err_stacktrace();
    assert(! "html_escape: cannot cast to Str");
    return NULL;
  }
  ident_init(&escape_ident, sym_1("HTML"), sym_1("escapes"));
  if (! ident_get(&escape_ident, &escape_tag)) {
    err_puts("html_escape: ident_get");
    assert(! "html_escape: ident_get");
    str_clean(&str);
    return NULL;
  }
  if (! tag_is_alist(&escape_tag)) {
    err_puts("html_escape: HTML.escapes is not an associative"
             " List");
    assert(!("html_escape: HTML.escapes is not an associative"
             " List"));
    tag_clean(&escape_tag);
    str_clean(&str);
    return NULL;
  }
  escape = escape_tag.data.td_plist;
  if (! buf_init_alloc(&buf, str.size * 8)) {
    err_puts("html_escape: buf_init_alloc");
    assert(! "html_escape: buf_init_alloc");
    tag_clean(&escape_tag);
    str_clean(&str);
    return NULL;
  }
  tag.type = TAG_STR;
  tag.data.td_str = (s_str) {0};
  s = str;
  while (str_read_character_utf8(&s, &c) > 0) {
    if (! str_init_character(&tag.data.td_str, c)) {
      err_puts("html_escape: invalid character");
      assert(! "html_escape: invalid character");
      goto ko;
    }
    replace = NULL;
    e = escape;
    while (e) {
      reserved = e->tag.data.td_ptuple->tag;
      if (reserved->type != TAG_STR) {
        err_puts("html_escape: HTML.escapes: reserved that is"
                 " not a Str");
        assert(!("html_escape: HTML.escapes: reserved that is"
                 " not a Str"));
        str_clean(&tag.data.td_str);
        goto ko;
      }
      if (! compare_tag(reserved, &tag)) {
        replace = e->tag.data.td_ptuple->tag + 1;
        if (replace->type != TAG_STR) {
          err_puts("html_escape: HTML.escapes: replacement that is"
                   " not a Str");
          assert(!("html_escape: HTML.escapes: replacement that is"
                   " not a Str"));
          str_clean(&tag.data.td_str);
          goto ko;
        }
        break;
      }
      e = list_next(e);
    }
    str_clean(&tag.data.td_str);
    if (replace) {
      if (buf_write_str(&buf, &replace->data.td_str) <= 0) {
        err_puts("html_escape: buf_write_str");
        assert(! "html_escape: buf_write_str");
        goto ko;
      }
    }
    else {
      if (buf_write_character_utf8(&buf, c) <= 0) {
        err_puts("html_escape: buf_write_character_utf8");
        assert(! "html_escape: buf_write_character_utf8");
        goto ko;
      }
    }
  }
  tag_clean(&escape_tag);
  str_clean(&str);
  s = (s_str) {0};
  if (buf_read_to_str(&buf, &s) < 0) {
    err_puts("html_escape: buf_read_to_str");
    assert(! "html_escape: buf_read_to_str");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  *dest = s;
  return dest;
 ko:
  tag_clean(&escape_tag);
  buf_clean(&buf);
  str_clean(&str);
  return NULL;
}
