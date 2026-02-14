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
#include "http.h"

s_tag * http_header_split (s_str *header, s_tag *dest)
{
  assert(header);
  assert(dest);
  s_tag *key;
  sw sep;
  s_tag tmp;
  s_tag *value;
  if ((sep = str_position_1(header, ": ")) < 0) {
    err_puts("http_header_split: missing separator");
    return NULL;
  }
  tag_init_ptuple(&tmp, 2);
  key = tmp.data.ptuple->tag;
  key->type = TAG_STR;
  if (! str_init_slice(&key->data.str, header, 0, sep)) {
    err_puts("http_header_split: str_init_slice 1");
    assert(! "http_header_split: str_init_slice 1");
    return NULL;
  }
  value = tmp.data.ptuple->tag + 1;
  value->type = TAG_STR;
  if (! str_init_slice(&value->data.str, header, sep + 2, -1)) {
    err_puts("http_header_split: str_init_slice 2");
    assert(! "http_header_split: str_init_slice 2");
    return NULL;
  }
  *dest = tmp;
  return dest;
}
