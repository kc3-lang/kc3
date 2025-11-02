/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "json.h"

sw json_buf_inspect (s_buf *buf, const s_tag *tag)
{
  const s_sym *type;
  assert(buf);
  assert(tag);
  switch (tag->type) {
  case TAG_MAP:
    return json_buf_inspect_map(buf, &tag->data.map);
  case TAG_STR:
    return buf_inspect_str(buf, &tag->data.str);
  case TAG_F32:
  case TAG_F64:
  case TAG_F80:
#if HAVE_FLOAT128
  case TAG_F128:
#endif
  case TAG_INTEGER:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    return json_buf_inspect_tag_number(buf, tag);
  case TAG_BOOL:
    return buf_inspect_bool(buf, tag->data.bool_);
  case TAG_VOID:
    return json_buf_inspect_void(buf);
  default:
    break;
  }
  err_write_1("json_buf_inspect: unknown tag type: ");
  tag_type(tag, &type);
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "json_buf_inspect: unknown tag type");
  return -1;
}

sw json_buf_inspect_map (s_buf *buf, const s_map *map)
{
  uw i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  s_str str;
  const s_sym *type = &g_sym_Str;
  if ((r = buf_write_1(buf, "{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  i = 0;
  while (i < map->count) {
    switch (map->key[i].type) {
    case TAG_STR:
      if ((r = buf_inspect_str(buf, &map->key[i].data.str)) < 0)
        return r;
      result += r;
      break;
    case TAG_PSYM:
      if ((r = buf_inspect_str(buf, &map->key[i].data.psym->str)) < 0)
        return r;
      result += r;
      break;
    default:
      if (! str_init_cast(&str, &type, map->key + i)) {
        err_puts("json_buf_inspect_map: cannot cast key to Str");
        assert(! "json_buf_inspect_map: cannot cast key to Str");
        return -1;
      }
      if ((r = buf_inspect_str(buf, &str)) < 0)
        return r;
      result += r;
      str_clean(&str);
    }
    if ((r = buf_write_1(buf, ": ")) < 0)
      return r;
    result += r;
    if ((r = json_buf_inspect(buf, map->value + i)) <= 0)
      return r;
    result += r;
    i++;
    if (i < map->count) {
      if ((r = buf_write_1(buf, ",\n")) < 0)
        return r;
      result += r;
    }
  }
  pretty_save_clean(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "}")) < 0)
    return -1;
  result += r;
  return result;
}

sw json_buf_inspect_map_size (s_pretty *pretty, const s_map *map)
{
  uw i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  s_str str;
  const s_sym *type = &g_sym_Str;
  assert(pretty);
  assert(map);
  if ((r = buf_write_1_size(pretty, "{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_from_column(pretty, 0);
  i = 0;
  while (i < map->count) {
    switch (map->key[i].type) {
    case TAG_STR:
      if ((r = buf_inspect_str_size(pretty, &map->key[i].data.str)) < 0)
        return r;
      result += r;
      break;
    case TAG_PSYM:
      if ((r = buf_inspect_str_size(pretty,
                                    &map->key[i].data.psym->str)) < 0)
        return r;
      result += r;
      break;
    default:
      if (! str_init_cast(&str, &type, map->key + i)) {
        err_puts("json_buf_inspect_map: cannot cast key to Str");
        assert(! "json_buf_inspect_map: cannot cast key to Str");
        return -1;
      }
      if ((r = buf_inspect_str_size(pretty, &str)) < 0)
        return r;
      result += r;
      str_clean(&str);
    }
    if ((r = buf_write_1_size(pretty, ": ")) < 0)
      return r;
    result += r;
    if ((r = json_buf_inspect_size(pretty, map->value + i)) <= 0)
      return r;
    result += r;
    i++;
    if (i < map->count) {
      if ((r = buf_write_1_size(pretty, ",\n")) < 0)
        return r;
      result += r;
    }
  }
  pretty_save_clean(&pretty_save, pretty);
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    return -1;
  result += r;
  return result;
}

sw json_buf_inspect_size (s_pretty *pretty, const s_tag *tag)
{
  const s_sym *type;
  assert(pretty);
  assert(tag);
  switch (tag->type) {
  case TAG_MAP:
    return json_buf_inspect_map_size(pretty, &tag->data.map);
  case TAG_STR:
    return buf_inspect_str_size(pretty, &tag->data.str);
  case TAG_F32:
  case TAG_F64:
  case TAG_F80:
#if HAVE_FLOAT128
  case TAG_F128:
#endif
  case TAG_INTEGER:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    return json_buf_inspect_tag_number_size(pretty, tag);
  case TAG_BOOL:
    return buf_inspect_bool_size(pretty, tag->data.bool_);
  case TAG_VOID:
    return json_buf_inspect_void_size(pretty);
  default:
    break;
  }
  err_write_1("json_buf_inspect: unknown tag type: ");
  tag_type(tag, &type);
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "json_buf_inspect: unknown tag type");
  return -1;
}

sw json_buf_inspect_tag_number (s_buf *buf, const s_tag *tag)
{
  s_integer i;
  sw r;
  const s_sym *type = &g_sym_Integer;
  assert(buf);
  assert(tag);
  if (! integer_init_cast(&i, &type, tag)) {
    err_write_1("json_buf_inspect_tag_number: cannot cast to"
                " Integer: ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "json_buf_inspect_tag_number: cannot cast to Integer: ");
    return -1;
  }
  r = buf_inspect_integer_decimal(buf, &i);
  integer_clean(&i);
  return r;
}

sw json_buf_inspect_tag_number_size (s_pretty *pretty, const s_tag *tag)
{
  s_integer i;
  sw r;
  const s_sym *type = &g_sym_Integer;
  assert(pretty);
  assert(tag);
  if (! integer_init_cast(&i, &type, tag)) {
    err_write_1("json_buf_inspect_tag_number: cannot cast to"
                " Integer: ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "json_buf_inspect_tag_number: cannot cast to Integer: ");
    return -1;
  }
  r = buf_inspect_integer_decimal_size(pretty, &i);
  integer_clean(&i);
  return r;
}

sw json_buf_inspect_void (s_buf *buf)
{
  assert(buf);
  return buf_write_1(buf, "null");
}

sw json_buf_inspect_void_size (s_pretty *pretty)
{
  assert(pretty);
  return buf_write_1_size(pretty, "null");
}

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
    case 'n':
      return json_buf_parse_null(buf, dest);
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
    k = &(*k)->next.data.plist;
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
    v = &(*v)->next.data.plist;
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
  if (buf_parse_tag_number(buf, dest) <= 0)
    return NULL;
  return dest;
}

s_tag * json_buf_parse_str (s_buf *buf, s_tag *dest)
{
  if (buf_parse_tag_str(buf, dest) <= 0)
    return NULL;
  return dest;
}

s_tag * json_from_str (const s_str *src, s_tag *dest)
{
  s_buf buf;
  buf_init_str_const(&buf, src);
  return json_buf_parse(&buf, dest);
}

s_str * json_to_str (const s_tag *tag, s_str *dest)
{
  s_pretty pretty = {0};
  sw size;
  s_buf tmp;
  size = json_buf_inspect_size(&pretty, tag);
  if (size < 0) {
    err_puts("json_to_str: buf_inspect_array_size error");
    assert(! "json_to_str: buf_inspect_array_size error");
    return NULL;
  }
  if (! buf_init_alloc(&tmp, size)) {
    err_puts("json_to_str: buf_init alloc");
    assert(! "json_to_str: buf_init_alloc");
    return NULL;
  }
  json_buf_inspect(&tmp, tag);
  if (tmp.wpos != tmp.size) {
    err_puts("json_to_str: tmp.wpos != tmp.size");
    assert(! "json_to_str: tmp.wpos != tmp.size");
    buf_clean(&tmp);
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
