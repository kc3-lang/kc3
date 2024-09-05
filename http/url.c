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
#include <libkc3/kc3.h>
#include "url.h"

s_str * url_escape (const s_str *src, s_str *dest)
{
  s_str *escapes;
  s_tag escapes_tag;
  s_ident ident;
  s_str s;
  sw size;
  ident_init(&ident, sym_1("URL"), sym_1("escapes"));
  if (! ident_get(&ident, &escapes_tag)) {
    err_puts("url_escape: missing URL.escapes");
    assert(! "url_escape: missing URL.escapes");
    return NULL;
  }
  if (escapes_tag.type != TAG_STR) {
    err_puts("url_escape: URL.escapes is not a Str");
    assert(! "url_escape: URL.escapes is not a Str");
    return NULL;
  }
  escapes = &escapes_tag.data.str;
  if ((size = url_escape_size(src)) < 0)
    return NULL;
  if (! size)
    return str_init_empty(dest);
  if (! buf_init_alloc(&buf, size))
    return NULL;
  s = *src;
  while (str_read_character_utf8(&s, &c) > 0) {
    if (str_character_position(escapes, c) >= 0) {
      if (buf_write_u8(buf, '%') < 0)
        goto clean;
      if (buf_write_u8_hex(buf, c) < 0)
        goto clean;
    }
    else if (buf_write_character_utf8(buf, c) < 0)
      goto clean;
  }
  tag_clean(&escapes_tag);
  if (! buf_to_str(&buf, dest)) {
    buf_clean(&buf);
    return NULL;
  }
  return dest;
 clean:
  tag_clean(&escapes_tag);
  return NULL;
}

sw url_escape_size (const s_str *src)
{
  s_str *escapes;
  s_tag escapes_tag;
  s_ident ident;
  sw r;
  sw result = 0;
  s_str s;
  ident_init(&ident, sym_1("URL"), sym_1("escapes"));
  if (! ident_get(&ident, &escapes_tag)) {
    err_puts("url_escape_size: missing URL.escapes");
    assert(! "url_escape_size: missing URL.escapes");
    return NULL;
  }
  if (escapes_tag.type != TAG_STR) {
    err_puts("url_escape_size: URL.escapes is not a Str");
    assert(! "url_escape_size: URL.escapes is not a Str");
    return NULL;
  }
  escapes = &escapes_tag.data.str;
  s = *src;
  while ((r = str_read_character_utf8(&s, &c)) > 0) {
    if (str_character_position(escapes, c) >= 0)
      result += 3;
    else
      result += r;
  }
  tag_clean(&escapes_tag);
  return result;
}

s_str * url_unescape (const s_str *url, s_str *dest)
{
  
}
