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
#include "assert.h"
#include "buf.h"
#include "ident.h"
#include "io.h"
#include "sh.h"
#include "str.h"
#include "tag.h"

s_str * sh_escape (const s_str *src, s_str *dest)
{
  bool      allowed;
  s_buf buf = {0};
  bool escape;
  s_ident ident;
  s_str     s;
  character s_c;
  s_str     t;
  character t_c;
  s_tag tag = {0};
  s_str tmp = {0};
  assert(src);
  assert(dest);
  if (! src->size)
    return str_init_1(dest, NULL, "\"\"");
  ident_init(&ident, sym_1("Sh"), sym_1("allowed"));
  if (! ident_get(&ident, &tag)) {
    err_puts("sh_escape: Sh.allowed is not defined");
    assert(! "sh_escape: Sh.allowed is not defined");
    return NULL;
  }
  if (tag.type != TAG_STR) {
    err_puts("sh_escape: Sh.allowed is not a Str");
    assert(! "sh_escape: Sh.allowed is not a Str");
    goto ko;
  }
  escape = false;
  s = *src;
  while (str_read_character_utf8(&s, &s_c) > 0) {
    allowed = false;
    t = tag.data.str;
    while (str_read_character_utf8(&t, &t_c) > 0) {
      if (s_c == t_c) {
        allowed = true;
        break;
      }
    }
    if (! allowed)
      escape = true;
  }
  tag_clean(&tag);
  if (! escape)
    return str_init_copy(dest, src);
  ident_init(&ident, sym_1("Sh"), sym_1("reserved"));
  if (! ident_get(&ident, &tag)) {
    err_puts("sh_escape: Sh.reserved is not defined");
    assert(! "sh_escape: Sh.reserved is not defined");
    return NULL;
  }
  if (tag.type != TAG_STR) {
    err_puts("sh_escape: Sh.reserved is not a Str");
    assert(! "sh_escape: Sh.reserved is not a Str");
    goto ko;
  }
  if (! buf_init_alloc(&buf, (src->size + 1) * 2))
    goto ko;
  if (buf_write_1(&buf, "\"") <= 0)
    goto ko;
  s = *src;
  while (str_read_character_utf8(&s, &s_c) > 0) {
    t = tag.data.str;
    while (str_read_character_utf8(&t, &t_c) > 0) {
      if (s_c == t_c) {
        if (buf_write_1(&buf, "\\") <= 0)
          goto ko;
        break;
      }
    }
    if (buf_write_character_utf8(&buf, s_c) <= 0)
      goto ko;
  }
  tag_clean(&tag);
  if (buf_write_1(&buf, "\"") <= 0)
    goto ko_buf;
  if (buf_read_to_str(&buf, &tmp) <= 0)
    goto ko_buf;
  buf_clean(&buf);
  *dest = tmp;
  return dest;
 ko:
  tag_clean(&tag);
 ko_buf:
  buf_clean(&buf);
  return NULL;
}
