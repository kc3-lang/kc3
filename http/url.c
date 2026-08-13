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
#include "url.h"

static void url_escape_ascii_table (const s_str *escapes,
                                    bool table[128])
{
  uw i;
  const u8 *p = (const u8 *) escapes->ptr.p_pu8;
  for (i = 0; i < escapes->size; i++)
    if (p[i] < 128)
      table[p[i]] = true;
}

static bool url_escape_character_is_escaped (const s_str *escapes,
                                              const bool table[128],
                                              character c)
{
  if (c < 128)
    return table[c];
  return str_character_position(escapes, c) >= 0;
}

s_str * url_escape (const s_str *src, s_str *dest)
{
  bool escaped_ascii[128] = {0};
  s_buf buf;
  character c;
  s_str *escapes;
  s_tag escapes_tag = {0};
  s_ident ident;
  sw r;
  s_str s;
  sw size;
  u8 u;
  ident_init(&ident, &g_sym_URL, &g_sym_escapes);
  if (! ident_get(&ident, &escapes_tag)) {
    err_puts("url_escape: missing URL.escapes");
    assert(! "url_escape: missing URL.escapes");
    return NULL;
  }
  if (escapes_tag.type != TAG_STR) {
    err_puts("url_escape: URL.escapes is not a Str");
    assert(! "url_escape: URL.escapes is not a Str");
    tag_clean(&escapes_tag);
    return NULL;
  }
  escapes = &escapes_tag.data.td_str;
  url_escape_ascii_table(escapes, escaped_ascii);
  s = *src;
  size = 0;
  while ((r = str_read_character_utf8(&s, &c)) > 0) {
    if (url_escape_character_is_escaped(escapes, escaped_ascii, c))
      size += 3;
    else
      size += r;
  }
  if (! size) {
    tag_clean(&escapes_tag);
    return str_init_empty(dest);
  }
  if (! buf_init_alloc(&buf, size)) {
    tag_clean(&escapes_tag);
    return NULL;
  }
  s = *src;
  while (str_read_character_utf8(&s, &c) > 0) {
    /* if (c == ' ') { 
         if (buf_write_u8(&buf, '+') < 0)
           goto clean;
       }
       else */
    if (url_escape_character_is_escaped(escapes, escaped_ascii, c)) {
      if (buf_write_u8(&buf, '%') < 0)
        goto clean;
      u = c;
      if (buf_u8_to_hex(&buf, &u) != 2)
        goto clean;
    }
    else if (buf_write_character_utf8(&buf, c) < 0)
      goto clean;
  }
  tag_clean(&escapes_tag);
  if (! buf_to_str(&buf, dest)) {
    buf_clean(&buf);
    return NULL;
  }
  return dest;
 clean:
  buf_clean(&buf);
  tag_clean(&escapes_tag);
  return NULL;
}

sw url_escape_size (const s_str *src)
{
  bool escaped_ascii[128] = {0};
  character c;
  s_str *escapes;
  s_tag escapes_tag = {0};
  s_ident ident;
  sw r;
  sw result = 0;
  s_str s;
  ident_init(&ident, &g_sym_URL, &g_sym_escapes);
  if (! ident_get(&ident, &escapes_tag)) {
    err_puts("url_escape_size: missing URL.escapes");
    assert(! "url_escape_size: missing URL.escapes");
    tag_clean(&escapes_tag);
    return -1;
  }
  if (escapes_tag.type != TAG_STR) {
    err_puts("url_escape_size: URL.escapes is not a Str");
    assert(! "url_escape_size: URL.escapes is not a Str");
    tag_clean(&escapes_tag);
    return -1;
  }
  escapes = &escapes_tag.data.td_str;
  url_escape_ascii_table(escapes, escaped_ascii);
  s = *src;
  while ((r = str_read_character_utf8(&s, &c)) > 0) {
    /* if (c == ' ')
         result += 1;
       else */
    if (url_escape_character_is_escaped(escapes, escaped_ascii, c))
      result += 3;
    else
      result += r;
  }
  tag_clean(&escapes_tag);
  return result;
}

s_str * url_unescape (const s_str *url, s_str *dest)
{
  character c;
  u8 digit[2];
  s_buf in;
  s_buf out;
  sw r;
  sw size;
  u8 u;
  assert(url);
  assert(dest);
  if ((size = url_unescape_size(url)) < 0)
    return NULL;
  if (! size)
    return str_init_empty(dest);
  if (! buf_init_alloc(&out, size))
    return NULL;
  buf_init_str_const(&in, url);
  while ((r = buf_read_character_utf8(&in, &c)) > 0) {
    if (c == '+') {
      if (buf_write_u8(&out, ' ') < 0)
        goto clean;
    }
    else if (c == '%') {
      if ((r = buf_parse_digit_hex(&in, &digit[0])) < 0)
        goto ok;
      if (! r) {
        buf_write_character_utf8(&out, '%');
        continue;
      }
      if ((r = buf_parse_digit_hex(&in, &digit[1])) < 0)
        goto ok;
      if (! r) {
        buf_write_character_utf8(&out, '%');
        buf_write_character_utf8
          (&out, g_kc3_base_hexadecimal.ptr.p_pchar[digit[0]]);
        continue;
      }
      u = digit[0] * 16 + digit[1];
      if (buf_write_u8(&out, u) < 0)
        goto clean;
    }
    else
      if (buf_write_character_utf8(&out, c) < 0)
        goto clean;
  }
 ok:
  buf_clean(&in);
  if (! buf_to_str(&out, dest)) {
    buf_clean(&out);
    return NULL;
  }
  return dest;
 clean:
  buf_clean(&in);
  buf_clean(&out);
  return NULL;
}

sw url_unescape_size (const s_str *url)
{
  character c;
  u8 digit[2];
  s_buf in;
  sw r;
  sw result = 0;
  assert(url);
  buf_init_str_const(&in, url);
  while ((r = buf_read_character_utf8(&in, &c)) > 0) {
    if (c == '%') {
      if ((r = buf_parse_digit_hex(&in, &digit[0])) < 0) {
	buf_clean(&in);
        return result;
      }
      if (! r) {
        result++;
        continue;
      }
      if ((r = buf_parse_digit_hex(&in, &digit[1])) < 0) {
	buf_clean(&in);
        return result;
      }
      if (! r) {
        result += 2;
        continue;
      }
      result++;
    }
    else
      result += r;
  }
  buf_clean(&in);
  return result;
}

s_tag * url_www_form_decode (const s_str *src, s_tag *dest)
{
  s_buf buf;
  s_str key = {0};
  s_list *list;
  s_list **tail;
  s_tag tmp = {0};
  s_str value = {0};
  buf_init_str_const(&buf, src);
  list = NULL;
  tail = &list;
  while (1) {
    if (buf_read_until_1_into_str(&buf, "=", &key) <= 0)
      break;
    if (buf_read_until_1_into_str(&buf, "&", &value) <= 0 &&
        buf_read_to_str(&buf, &value) < 0) {
      str_clean(&key);
      goto clean;
    }
    if (! key.size) {
      str_clean(&key);
      str_clean(&value);
      continue;
    }
    if (! (*tail = list_new_ptuple(2, NULL))) {
      str_clean(&key);
      str_clean(&value);
      goto clean;
    }
    (*tail)->tag.data.td_ptuple->tag[0].type = TAG_STR;
    (*tail)->tag.data.td_ptuple->tag[1].type = TAG_STR;
    if (! url_unescape
        (&key, &(*tail)->tag.data.td_ptuple->tag[0].data.td_str)) {
      str_clean(&key);
      str_clean(&value);
      goto clean;
    }
    if (! url_unescape
        (&value, &(*tail)->tag.data.td_ptuple->tag[1].data.td_str)) {
      str_clean(&key);
      str_clean(&value);
      goto clean;
    }
    str_clean(&key);
    str_clean(&value);
    tail = &(*tail)->next.data.td_plist;
  }
  buf_clean(&buf);
  tmp.type = TAG_PLIST;
  tmp.data.td_plist = list;
  *dest = tmp;
  return dest;
 clean:
  buf_clean(&buf);
  list_delete_all(list);
  return NULL;
}
