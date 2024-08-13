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
#include "err.h"
#include "tag.h"

s_tag * json_buf_parse_map (const s_buf *buf, s_tag *dest) {
  s_buf_save save;
  assert(buf);
  assert(dest);
  dest->type = TAG_MAP
  dest->data.map = map_new();
  buf_save_init(buf, &save);
  if (! buf_parse_map(buf, dest->data.map)) {
    buf_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}

s_tag * json_buf_parse_list (const s_buf *buf, s_tag *dest) {
  s_buf_save save;
  assert(buf);
  assert(dest);
  dest->type = TAG_LIST;
  dest->data.list = list_new();
  buf_save_init(buf, &save);
  if (! buf_parse_list(buf, dest->data.list)) {
    buf_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}

s_tag * json_buf_parse_str (const s_buf *buf, s_tag *dest) {
  s_buf_save save;
  assert(buf);
  assert(dest);
  dest->type = TAG_STR;
  dest->data.str = str_new();
  buf_save_init(buf, &save);
  if (! buf_parse_str(buf, dest->data.str)) {
    buf_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}

s_tag * json_buf_parse_numbers (const s_buf *buf, s_tag *dest) {
  assert(buf);
  assert(dest);
  if (! buf_parse_tag_number(buf, dest)) {
    buf_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}

s_tag * json_buf_parse_boolean (const s_buf *buf, s_tag *dest) {
  assert(buf);
  assert(dest);
  if (buf_peek(buf) == 't') {
    dest->type = TAG_BOOL;
    dest->data.boolean = true;
    buf_skip(buf, 4);
  } else {
    dest->type = TAG_BOOL;
    dest->data.boolean = false;
    buf_skip(buf, 5);
  }
  return dest;
}

static bool parse_json(const s_buf *buf, s_tag *dest) {
  assert(buf);
  assert(dest);
  switch (buf_peek(buf)) {
    case '{':
      json_buf_parse_map(buf, dest);
      if (buf_peek(buf) != '}') {
        errputs("json_parse: expected '}'");
        return false;
      }
      break;
    case '[':
      json_buf_parse_list(buf, dest);
        if (buf_peek(buf) != ']') {
          errputs("json_parse: expected ']'");
          return false;
       }
      break;
    case '"':
      json_buf_parse_str(buf, dest);
      if (buf_peek(buf) != '"') {
        errputs("json_parse: expected '\"'");
        return false;
      }
      break;
    case '0' ... '9':
      json_buf_parse_numbers(buf, dest);
      break;
    case 't':
    case 'f':
      json_buf_parse_boolean(buf, dest);
      break;
    case 'n':
      dest->type = TAG_NULL;
      break;
    default:
      return false;
  }
  return true;
}

s_tag *json_parse(const s_buf *buf, s_tag *dest) {
  assert(buf);
  assert(dest);
  s_buf_save save;
  buf_save_init(buf, &save);
  if (! parse_json(buf, dest)) {
    buf_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}
