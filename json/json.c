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



s_tag * json_buf_parse (s_buf *buf, s_tag *dest)
{
  character c;
  sw r;
  assert(buf);
  assert(dest);
  if ((r = buf_peek_character_utf8(buf, &c)) < 0)
    return NULL;
  switch (c) {
    case '{':
      return json_buf_parse_map(buf, dest);
    case '"':
      return json_buf_parse_str(buf, dest);
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
      return json_buf_parse_number(buf, dest);
      break;
    case 't':
    case 'f':
      return json_buf_parse_bool(buf, dest);
      break;
    default:
      return NULL;
  }
  return NULL;
}

s_tag * json_buf_parse_bool (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_tag_bool(buf, dest)) <= 0)
    return NULL;
  return dest;
}

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
  keys = NULL;
  k = &keys;
  values = NULL;
  v = &values;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "{")) <= 0)
    goto clean;
  while (1) {
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    if ((r = buf_read_1(buf, "}")) < 0)
      goto restore;
    if (r > 0)
      break;
    *k = list_new(NULL);
    if (! *k)
      goto restore;
    if ((r = buf_parse_tag_str(buf, &(*k)->tag)) <= 0)
      goto restore;
    k = &(*k)->next.data.list;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    if ((r = buf_read_1(buf, ":")) <= 0)
      goto restore;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    *v = list_new(NULL);
    if (! *v)
      goto restore;
    if (! json_buf_parse(buf, &(*v)->tag))
      goto restore;
    v = &(*v)->next.data.list;
  }
  if (! tag_init_map_from_lists(dest, keys, values))
    goto restore;
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_clean(buf, &save);
  return dest;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_clean(buf, &save);
  return NULL;
}

s_tag * json_buf_parse_null (s_buf *buf, s_tag *dest)
{
  character c;
  sw r;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "null")) <= 0)
    goto clean;
  if (buf_peek_character_utf8(buf, &c) > 0 &&
      ! ident_character_is_reserved(c)) {
    r = 0;
    goto restore;
  }
  tag_init_void(dest);
  buf_save_clean(buf, &save);
  return dest;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return NULL;
}

s_tag * json_buf_parse_number (s_buf *buf, s_tag *dest)
{
  assert(buf);
  assert(dest);
  if (! buf_parse_tag_number(buf, dest))
    return NULL;
  return dest;
}

s_tag * json_buf_parse_str (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_parse_tag_str(buf, dest)) <= 0)
    return NULL;
  return dest;
}

s_tag * json_from_str (const s_str *src, s_tag *dest)
{
  s_buf buf;
  buf_init_str_const(&buf, src);
  return json_buf_parse(&buf, dest);
}
