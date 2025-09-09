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
#include "pdf_buf_parse.h"

sw pdf_buf_parse (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_tag tmp = {0};
  if ((r = buf_ignore_spaces(buf)) < 0)
    return r;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto ok;
  result += r;
  if ((r = pdf_buf_parse_indirect_object(buf, &tmp.data.tuple)) < 0)
    goto ok;
  if (r) {
    result += r;
    tmp.type = TAG_TUPLE;
    goto ok;
  }
  if ((r = pdf_buf_parse_bool(buf, &tmp.data.bool_)) < 0)
    goto ok;
  if (r) {
    result += r;
    tmp.type = TAG_BOOL;
    goto ok;
  }
  if ((r = pdf_buf_parse_number(buf, &tmp)) < 0)
    goto ok;
  if (r) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_dictionnary(buf, &tmp.data.map)) < 0)
    goto ok;
  if (r) {
    result += r;
    tmp.type = TAG_MAP;
    goto ok;
  }
  if ((r = pdf_buf_parse_string(buf, &tmp)) < 0)
    goto ok;
  if (r) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_name(buf, &tmp.data.psym)) < 0)
    goto ok;
  if (r) {
    tmp.type = TAG_PSYM;
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_array(buf, &tmp.data.plist)) < 0)
    goto ok;
  if (r) {
    tmp.type = TAG_PLIST;
    result += r;
    goto ok;
  }
 ok:
  if (result)
    *dest = tmp;
  return result;
}

sw pdf_buf_parse_1 (s_buf *buf, const char *pchar)
{
  bool end;
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, pchar)) <= 0)
    goto clean;
  result += r;
  if ((r = pdf_buf_parse_object_end(buf, &end)) < 0)
    goto restore;
  if (! end) {
    r = 0;
    goto restore;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_array (s_buf *buf, p_list *dest)
{
  sw r;
  sw result = 0;
  p_list *tail;
  p_list tmp = NULL;
  s_buf_save save = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "[")) <= 0)
    goto clean;
  result += r;
  tail = &tmp;
  while (1) {
    if ((r = buf_read_1(buf, "]")) < 0)
      goto restore;
    result += r;
    if (r)
      break;
    if (! (*tail = list_new(NULL))) {
      r = -2;
      goto restore;
    }
    if ((r = pdf_buf_parse(buf, &(*tail)->tag)) <= 0)
      goto restore;
    result += r;
    tail = &(*tail)->next.data.plist;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  list_delete_all(tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_bool (s_buf *buf, bool *dest)
{
  bool end;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  bool tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "true")) > 0)
    tmp = true;
  else if ((r = buf_read_1(buf, "false")) > 0)
    tmp = false;
  else
    goto clean;
  result += r;
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = 0;
    goto restore;
  }
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_comment (s_buf *buf)
{
  sw r;
  sw result = 0;
  s_str str = {0};
  assert(buf);
  if ((r = buf_read_1(buf, "%")) <= 0)
    return r;
  result += r;
  if ((r = buf_read_until_1_into_str(buf, "\n", &str)) < 0)
    return result;
  str_clean(&str);
  return result;
}

sw pdf_buf_parse_comments (s_buf *buf)
{
  sw r;
  sw result = 0;
  assert(buf);
  while (1) {
    if ((r = pdf_buf_parse_comment(buf)) <= 0)
      break;
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      break;
    result += r;
  }
  if (r < 0)
    return r;
  return result;
}

sw pdf_buf_parse_dictionnary (s_buf *buf, s_map *dest)
{
  p_list  keys = NULL;
  p_list *keys_tail = &keys;
  const s_sym *name;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_map tmp = {0};
  p_list  values = NULL;
  p_list *values_tail = &values;
  buf_save_init(buf, &save);
  if ((r = pdf_buf_parse_1(buf, "<<")) <= 0)
    goto clean;
  result += r;
  while (1) {
    if ((r = pdf_buf_parse_1(buf, ">>")) < 0)
      goto restore;
    if (r) {
      result += r;
      goto ok;
    }
    if ((r = pdf_buf_parse_name(buf, &name)) <= 0)
      goto restore;
    result += r;
    if (! (*keys_tail = list_new_psym(name, NULL))) {
      r = -1;
      goto restore;
    }
    if (! (*values_tail = list_new(NULL))) {
      r = -1;
      goto restore;
    }
    if ((r = pdf_buf_parse(buf, &(*values_tail)->tag)) <= 0)
      goto restore;
    result += r;
  }
 ok:
  if (! map_init_from_lists(&tmp, keys, values)) {
    r = -1;
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  list_delete_all(values);
  list_delete_all(keys);
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_float (s_buf *buf, f32 *dest)
{
  u8 digit;
  bool end;
  f32 i;
  bool negative = false;
  sw r;
  sw result = 0;
  s_buf_save save;
  f32 tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "+")) < 0)
    goto clean;
  if (r)
    result += r;
  else {
    if ((r = buf_read_1(buf, "-")) < 0)
      goto clean;
    if (r) {
      negative = true;
      result += r;
    }
  }
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto restore;
  result += r;
  tmp = digit;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp = tmp * 10 + digit;
  }
  if (r < 0 ||
      (r = buf_read_1(buf, ".")) <= 0)
    goto ok;
  result += r;
  i = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    tmp += (f32) digit / i;
    i *= 10;
    result += r;
  }
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = -1;
    goto restore;
  }
  result += r;
 ok:
  if (negative)
    tmp = -tmp;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_indirect_object (s_buf *buf, s_tuple *dest)
{
  s32 generation_number;
  s32 object_number;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  const s_sym *sym_indirect_object = sym_1("indirect_object");
  s_tuple tmp = {0};
  buf_save_init(buf, &save);
  if ((r = pdf_buf_parse_integer(buf, &object_number)) <= 0)
    goto clean;
  result += r;
  if ((r = pdf_buf_parse_integer(buf, &generation_number)) <= 0)
    goto restore;
  result += r;
  if ((r = pdf_buf_parse_1(buf, "obj")) < 0)
    goto restore;
  if (r) {
    result += r;
    if (! tuple_init(&tmp, 4)) {
      r = -1;
      goto restore;
    }
    if ((r = pdf_buf_parse(buf, tmp.tag + 3)) <= 0)
      goto restore;
    goto ok;
  }
  if ((r = pdf_buf_parse_1(buf, "R")) < 0)
    goto restore;
  if (r) {
    result += r;
    if (! tuple_init(&tmp, 3)) {
      r = -1;
      goto restore;
    }
    goto ok;
  }
 ok:
  tag_init_psym(tmp.tag, sym_indirect_object);
  tag_init_s32(tmp.tag + 1, object_number);
  tag_init_s32(tmp.tag + 2, generation_number);
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  tuple_clean(&tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_integer (s_buf *buf, s32 *dest)
{
  bool end;
  bool negative = false;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s32 tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "+")) < 0)
    goto clean;
  if (r)
    result += r;
  else {
    if ((r = buf_read_1(buf, "-")) < 0)
      goto clean;
    if (r) {
      negative = true;
      result += r;
    }
  }
  if ((r = buf_parse_s32_decimal(buf, negative, &tmp)) <= 0)
    goto restore;
  result += r;
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = -1;
    goto restore;
  }
  if (negative)
    tmp = -tmp;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_name (s_buf *buf, p_sym *dest)
{
  char a[BUF_SIZE];
  character c;
  u8 d;
  u8 d1;
  bool end;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str str = {0};
  p_sym tmp = {0};
  s_buf tmp_buf;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "/")) <= 0)
    goto clean;
  result += r;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
      result += r;
    if (end)
      goto ok;
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      break;
    result += r;
    if (c == '#') {
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        break;
      result += r;
      if (c >= '0' && c <= '9')
        d = c - '0';
      else if (c >= 'A' && c <= 'F')
        d = c - 'A';
      else if (c >= 'a' && c <= 'f')
        d = c - 'a';
      else {
        r = 0;
        goto restore;
      }
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        break;
      result += r;
      if (c >= '0' && c <= '9')
        d1 = c - '0';
      else if (c >= 'A' && c <= 'F')
        d1 = c - 'A';
      else if (c >= 'a' && c <= 'f')
        d1 = c - 'a';
      else {
        r = 0;
        goto restore;
      }
      c = d * 0x10 + d1;
    }
    if ((r = buf_write_character_utf8(&tmp_buf, c)) <= 0)
      goto restore;
  }
 ok:
  if (! buf_read_to_str(&tmp_buf, &str)) {
    r = -2;
    goto clean;
  }
  if (! (tmp = str_to_sym(&str))) {
    r = -2;
    str_clean(&str);
    goto clean;
  }
  str_clean(&str);
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_number (s_buf *buf, s_tag *dest)
{
  sw r;
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  if ((r = pdf_buf_parse_integer(buf, &tmp.data.s32)) < 0) {
    tmp.type = TAG_S32;
    return r;
  }
  if (r)
    goto ok;
  if ((r = pdf_buf_parse_float(buf, &tmp.data.f32)) < 0) {
    tmp.type = TAG_F32;
    return r;
  }
  if (r)
    goto ok;
  return 0;
 ok:
  *dest = tmp;
  return r;
}

sw pdf_buf_parse_object_end (s_buf *buf, bool *end)
{
  character c;
  sw r;
  sw result = 0;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto end;
  result += r;
  if ((r = pdf_buf_parse_comments(buf)) < 0)
    goto end;
  result += r;
  if ((r = buf_peek_character_utf8(buf, &c)) < 0)
    goto end;
  if (r && pdf_character_is_delimiter(c))
    goto end;
  *end = result > 0 ? true : false;
  return result;
 end:
  *end = true;
  return result;
}

sw pdf_buf_parse_string (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str str = {0};
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = pdf_buf_parse_string_paren(buf, &str)) < 0)
    goto clean;
  if (r) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_string_hex(buf, &str)) < 0)
    goto clean;
  if (r) {
    result += r;
    goto ok;
  }
  r = 0;
  goto clean;
 ok:
  if (false) {
    err_write_1("pdf_buf_parse_string: str = ");
    err_inspect_str(&str);
    err_write_1("\n");
  }
  if (! str_parse_eval(&str, &tmp)) {
    err_puts("pdf_buf_parse_string: str_parse_eval");
    assert(! "pdf_buf_parse_string: str_parse_eval");
    r = -1;
    str_clean(&str);
    goto restore;
  }
  str_clean(&str);
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_string_hex (s_buf *buf, s_str *dest)
{
  char a[BUF_SIZE] = {0};
  character c;
  u8 d;
  u8 d1;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str tmp = {0};
  s_buf tmp_buf = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "<")) <= 0)
    goto clean;
  result += r;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
    result += r;
    if (c == '>')
      break;
    if (c >= '0' && c <= '9')
      d = c - '0';
    else if (c >= 'a' && c <= 'f')
      d = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      d = c - 'A' + 10;
    else
      goto restore;
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
    result += r;
    if (c >= '0' && c <= '9')
      d1 = c - '0';
    else if (c >= 'a' && c <= 'f')
      d1 = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      d1 = c - 'A' + 10;
    else
      goto restore;
    if ((r = buf_write_u8(&tmp_buf, (d << 4) + d1)) <= 0)
      goto restore;
  }
  if (! buf_read_to_str(&tmp_buf, &tmp)) {
    r = -2;
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_clean(&tmp_buf);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

// parentheses must be balanced or backslashed
sw pdf_buf_parse_string_paren (s_buf *buf, s_str *dest)
{
  char a[BUF_SIZE] = {0};
  character c;
  character c1;
  character c2;
  s32 paren = 0;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str tmp = {0};
  s_buf tmp_buf = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  paren++;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
    result += r;
    if (c == '(')
      paren++;
    else if (c == ')') {
      paren--;
      if (! paren)
        goto ok;
    }
    else if (c == '\\') {
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        goto restore;
      result += r;
      switch (c) {
      case 'n':  c = '\n'; break;
      case 'r':  c = '\r'; break;
      case 't':  c = '\t'; break;
      case 'b':  c = '\b'; break;
      case 'f':  c = '\f'; break;
      case '(':  c = '(';  break;
      case ')':  c = ')';  break;
      case '\\': c = '\\'; break;
      case '0': case '1': case '2': case '3':
        if ((r = buf_read_character_utf8(buf, &c1)) <= 0)
          break;
        if (c1 < '0' || c1 > '7') {
          r = -1;
          break;
        }
        result += r;
        if ((r = buf_read_character_utf8(buf, &c2)) <= 0)
          break;
        if (c2 < '0' || c2 > '7') {
          r = -1;
          break;
        }
        result += r;
        c = (c - '0') * 0100 + (c1 - '0') * 010 + (c2 - '0');
        break;
      case '\n':
        goto next;
      }
    }
    if ((r = buf_write_character_utf8(&tmp_buf, c)) <= 0)
      goto restore;
  next:
    continue;
  }
 ok:
  if ((r = buf_read_to_str(&tmp_buf, &tmp)) < 0)
    goto restore;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

bool pdf_character_is_delimiter (character c)
{
  return (c == ']' ||
          c == '%' ||
          character_is_space(c));
}
