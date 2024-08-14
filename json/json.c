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

/*s_tag * json_buf_parse_null (const s_buf *buf, s_tag *dest) {
  character c;
  sw r;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "void")) <= 0)
    goto clean;
  if (buf_peek_character_utf8(buf, &c) > 0 &&
      ! ident_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}*/

s_tag * json_buf_parse_map (s_buf *buf, s_tag *dest)
{
  s_list **k;
  s_list  *keys;
  sw r;
  s_buf_save save;
  s_list **v;
  s_list  *values;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  keys = NULL;
  k = &keys;
  values = NULL;
  v = &values;
  while (1) {
    if ((r = buf_ignore_spaces(buf)) <= 0)
      goto restore;
    if ((r = buf_read_1(buf, "}")) < 0)
      goto restore;
    if (r > 0)
      break;
    buf_parse_tag_str(buf, &(*k)->tag);
    buf_parse_tag(buf, &(*v)->tag);
  }
  dest->type = TAG_MAP;
  if (! map_init_from_lists(&dest->data.map, keys, values))
    goto restore;
  list_delete_all(keys);
  list_delete_all(values);
  return dest;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_clean(buf, &save);
  return NULL;
}

s_tag * json_buf_parse_numbers (s_buf *buf, s_tag *dest)
{
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if (! buf_parse_tag_number(buf, dest)) {
    buf_save_restore_rpos(buf, &save);
    return NULL;
  }
  return dest;
}

s_tag * json_buf_parse (s_buf *buf, s_tag *dest)
{
  character c;
  sw r;
  s_buf_save save;
  s_tag tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r =buf_peek_character_utf8(buf, &c)) < 0)
    return NULL;
  switch (c) {
    case '{':
      json_buf_parse_map(buf, &tmp);
      break;
    case '"':
      buf_parse_tag_str(buf, &tmp);
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
      json_buf_parse_numbers(buf, &tmp);
      break;
    case 't':
    case 'f':
      buf_parse_tag_bool(buf, &tmp);
      break;
    default:
      return NULL;
  }
  *dest = tmp;
  return dest;
}

s_tag * json_from_str (const s_str *src, s_tag *dest)
{
  s_buf buf;
  buf_init_str_const(&buf, src);
  return json_buf_parse(&buf, dest);
}
