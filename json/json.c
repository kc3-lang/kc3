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
#include "json.h"

sw json_buf_inspect (s_buf *buf, const s_tag *tag)
{
  const s_sym *type;
  assert(buf);
  assert(tag);
  switch (tag->type) {
  case TAG_MAP:
    return json_buf_inspect_map(buf, &tag->data.td_map);
  case TAG_STR:
    return buf_inspect_str(buf, &tag->data.td_str);
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
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
    return buf_inspect_bool(buf, tag->data.td_bool_);
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
      if ((r = buf_inspect_str(buf, &map->key[i].data.td_str)) < 0)
        return r;
      result += r;
      break;
    case TAG_PSYM:
      if ((r = buf_inspect_str(buf, &map->key[i].data.td_psym->str)) < 0)
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
      if ((r = buf_inspect_str_size(pretty, &map->key[i].data.td_str)) < 0)
        return r;
      result += r;
      break;
    case TAG_PSYM:
      if ((r = buf_inspect_str_size(pretty,
                                    &map->key[i].data.td_psym->str)) < 0)
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
    return json_buf_inspect_map_size(pretty, &tag->data.td_map);
  case TAG_STR:
    return buf_inspect_str_size(pretty, &tag->data.td_str);
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
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
    return buf_inspect_bool_size(pretty, tag->data.td_bool_);
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

static sw json_buf_peek_non_space (s_buf *buf, u8 *dest)
{
  sw r;
  while ((r = buf_peek_u8(buf, dest)) > 0) {
    if (*dest != ' ' && *dest != '\t' &&
        *dest != '\r' && *dest != '\n')
      break;
    if ((r = buf_ignore(buf, 1)) <= 0)
      return r;
  }
  return r;
}

s_tag * json_buf_parse (s_buf *buf, s_tag *dest)
{
  u8 c;
  s_tag *result = NULL;
  s_buf_save save;
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if (json_buf_peek_non_space(buf, &c) <= 0)
    goto restore;
  switch (c) {
  case '[':
    result = json_buf_parse_list(buf, &tmp);
    break;
  case '{':
    result = json_buf_parse_map(buf, &tmp);
    break;
  case '"':
    result = json_buf_parse_str(buf, &tmp);
    break;
  case '0': case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9': case '-':
    result = json_buf_parse_number(buf, &tmp);
    break;
  case 't':
  case 'f':
    result = json_buf_parse_bool(buf, &tmp);
    break;
  case 'n':
    result = json_buf_parse_null(buf, &tmp);
    break;
  default:
    goto restore;
  }
  if (! result)
    goto restore;
  dest->type = tmp.type;
  dest->data = tmp.data;
  buf_save_clean(buf, &save);
  return dest;
 restore:
  tag_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return NULL;
}

s_tag * json_buf_parse_bool (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = buf_read_1(buf, "false")) > 0)
    return tag_init_bool(dest, false);
  if ((r = buf_read_1(buf, "true")) > 0)
    return tag_init_bool(dest, true);
  return NULL;
}

s_tag * json_buf_parse_list (s_buf *buf, s_tag *dest)
{
  u8 c;
  sw r;
  s_buf_save save;
  p_list *tail;
  p_list tmp = NULL;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "[")) <= 0)
    goto clean;
  if (json_buf_peek_non_space(buf, &c) <= 0)
    goto restore;
  if (c == ']') {
    if (buf_ignore(buf, 1) <= 0)
      goto restore;
    goto ok;
  }
  tail = &tmp;
  while (1) {
    if (! (*tail = list_new(NULL)))
      goto restore;
    if (! json_buf_parse(buf, &(*tail)->tag))
      goto restore;
    tail = &(*tail)->next.data.td_plist;
    if (json_buf_peek_non_space(buf, &c) <= 0)
      goto restore;
    if (c == ']') {
      if (buf_ignore(buf, 1) <= 0)
        goto restore;
      break;
    }
    if (c != ',' || buf_ignore(buf, 1) <= 0)
      goto restore;
  }
 ok:
  buf_save_clean(buf, &save);
  return tag_init_plist(dest, tmp);
 restore:
  err_puts("json_buf_parse_list: invalid list");
  buf_save_restore_rpos(buf, &save);
 clean:
  list_delete_all(tmp);
  buf_save_clean(buf, &save);
  return NULL;
}

s_tag * json_buf_parse_map (s_buf *buf, s_tag *dest)
{
  u8 c;
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
  if (json_buf_peek_non_space(buf, &c) <= 0)
    goto restore;
  if (c == '}') {
    if (buf_ignore(buf, 1) <= 0)
      goto restore;
    goto ok;
  }
  while (1) {
    *k = list_new(NULL);
    if (! *k)
      goto restore;
    if (! json_buf_parse_str(buf, &(*k)->tag))
      goto restore;
    k = &(*k)->next.data.td_plist;
    if (json_buf_peek_non_space(buf, &c) <= 0 ||
        c != ':' || buf_ignore(buf, 1) <= 0 ||
        json_buf_peek_non_space(buf, &c) <= 0)
      goto restore;
    *v = list_new(NULL);
    if (! *v)
      goto restore;
    if (! json_buf_parse(buf, &(*v)->tag))
      goto restore;
    v = &(*v)->next.data.td_plist;
    if (json_buf_peek_non_space(buf, &c) <= 0)
      goto restore;
    if (c == '}') {
      if (buf_ignore(buf, 1) <= 0)
        goto restore;
      break;
    }
    if (c != ',' || buf_ignore(buf, 1) <= 0 ||
        json_buf_peek_non_space(buf, &c) <= 0)
      goto restore;
  }
 ok:
  if (! tag_init_map_from_lists(dest, keys, values)) {
    err_puts("json_buf_parse_map: tag_init_map_from_lists");
    goto restore;
  }
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_clean(buf, &save);
  return dest;
 restore:
  err_puts("json_buf_parse_map: invalid map");
  err_inspect_buf(buf);
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
  tag_init(dest);
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

static bool json_buf_parse_str_hex4 (s_buf *buf, character *dest)
{
  u8 b;
  character c = 0;
  uw i;
  for (i = 0; i < 4; i++) {
    if (buf_read_u8(buf, &b) <= 0)
      return false;
    if (b >= '0' && b <= '9')
      c = c * 16 + b - '0';
    else if (b >= 'A' && b <= 'F')
      c = c * 16 + b - 'A' + 10;
    else if (b >= 'a' && b <= 'f')
      c = c * 16 + b - 'a' + 10;
    else
      return false;
  }
  *dest = c;
  return true;
}

static bool json_buf_parse_str_character (s_buf *buf, character *dest)
{
  u8 b;
  character c;
  uw count;
  uw i;
  character low;
  character min;
  if (buf_read_u8(buf, &b) <= 0 || b < 0x20)
    return false;
  if (b == '\\') {
    if (buf_read_u8(buf, &b) <= 0)
      return false;
    switch (b) {
    case '"': case '\\': case '/': c = b; break;
    case 'b': c = '\b'; break;
    case 'f': c = '\f'; break;
    case 'n': c = '\n'; break;
    case 'r': c = '\r'; break;
    case 't': c = '\t'; break;
    case 'u':
      if (! json_buf_parse_str_hex4(buf, &c))
        return false;
      if (c >= 0xD800 && c <= 0xDBFF) {
        if (buf_read_u8(buf, &b) <= 0 || b != '\\' ||
            buf_read_u8(buf, &b) <= 0 || b != 'u' ||
            ! json_buf_parse_str_hex4(buf, &low) ||
            low < 0xDC00 || low > 0xDFFF)
          return false;
        c = 0x10000 + ((c - 0xD800) << 10) + low - 0xDC00;
      }
      else if (c >= 0xDC00 && c <= 0xDFFF)
        return false;
      break;
    default:
      return false;
    }
  }
  else if (b < 0x80)
    c = b;
  else {
    /* Decode only shortest-form UTF-8 Unicode scalar values. */
    if (b >= 0xC2 && b <= 0xDF) {
      c = b & 0x1F;
      count = 1;
      min = 0x80;
    }
    else if (b >= 0xE0 && b <= 0xEF) {
      c = b & 0x0F;
      count = 2;
      min = 0x800;
    }
    else if (b >= 0xF0 && b <= 0xF4) {
      c = b & 0x07;
      count = 3;
      min = 0x10000;
    }
    else
      return false;
    for (i = 0; i < count; i++) {
      if (buf_read_u8(buf, &b) <= 0 || (b & 0xC0) != 0x80)
        return false;
      c = (c << 6) | (b & 0x3F);
    }
    if (c < min || c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
      return false;
  }
  *dest = c;
  return true;
}

s_tag * json_buf_parse_str (s_buf *buf, s_tag *dest)
{
  u8 b;
  character c;
  uw capacity = 0;
  char *p = NULL;
  uw pass;
  sw r;
  s_buf_save save;
  uw size = 0;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  /* Validate and measure before allocating, then decode the saved input. */
  for (pass = 0; pass < 2; pass++) {
    buf_save_restore_rpos(buf, &save);
    if (buf_read_u8(buf, &b) <= 0 || b != '"')
      goto restore;
    size = 0;
    while (1) {
      if (buf_peek_u8(buf, &b) <= 0)
        goto restore;
      if (b == '"') {
        if (buf_read_u8(buf, &b) <= 0)
          goto restore;
        break;
      }
      if (! json_buf_parse_str_character(buf, &c) ||
          (r = character_utf8_size(c)) <= 0 ||
          size > STR_MAX - (uw) r)
        goto restore;
      if (p) {
        if ((uw) r > capacity - size)
          goto restore;
        character_utf8(c, p + size);
      }
      size += r;
    }
    if (! pass) {
      capacity = size;
      if (! (p = alloc(capacity + 1)))
        goto restore;
    }
  }
  if (size != capacity)
    goto restore;
  p[size] = 0;
  buf_save_clean(buf, &save);
  return tag_init_str(dest, p, size, p);
 restore:
  alloc_free(p);
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return NULL;
}

s_tag * json_from_str (const s_str *src, s_tag *dest)
{
  s_buf buf;
  u8 c;
  s_tag tmp = {0};
  buf_init_str_const(&buf, src);
  if (! json_buf_parse(&buf, &tmp))
    goto ko;
  json_buf_peek_non_space(&buf, &c);
  if (buf.rpos != buf.wpos)
    goto ko;
  buf_clean(&buf);
  *dest = tmp;
  return dest;
 ko:
  err_puts("json_from_str: json_buf_parse");
  tag_clean(&tmp);
  buf_clean(&buf);
  return NULL;
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
