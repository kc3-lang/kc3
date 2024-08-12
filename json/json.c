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
#include "json.h"
#include "buf.h"
#include "buf_parse.h"
#include "tag.h"

static bool parse_json(const s_buf *buf, s_tag *dest) {
  assert(buf);
  assert(dest);
  if (buf_parse_tag_number(buf, dest) == 0)
    return false;
  return true;
}

s_tag *json_parse(const s_buf *buf, s_tag *dest) {
  assert(buf);
  assert(dest);
  s_buf_save save;
  buf_save_init(buf, &save);
  if (!parse_json(buf, dest)) {
    buf_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}