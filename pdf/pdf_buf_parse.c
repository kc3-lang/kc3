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
  if ((r = buf_parse_comments(buf)) < 0)
    return r;
  result += r;
  *dest = tmp;
  r = result;
  return r;
}

sw pdf_buf_parse_bool (s_buf *buf, bool *dest)
{
  sw r;
  sw result = 0;
  bool tmp;
  if ((r = buf_read_1(buf, "true")) > 0)
    tmp = true;
  else if ((r = buf_read_1(buf, "false")) > 0)
    tmp = false;
  else
    return r;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    return result;
  if (! r)
    return r;
  result += r;
  *dest = tmp;
  return result;
}

sw pdf_buf_parse_comment (s_buf *buf)
{
  sw r;
  sw result = 0;
  s_str str = {0};
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

sw pdf_buf_parse_float (s_buf *buf, f32 *dest)
{
  u8 digit;
  f32 i;
  bool negative = false;
  sw r;
  sw result = 0;
  f32 tmp;
  if ((r = buf_read_1(buf, "+")) < 0)
    return r;
  if (r)
    result += r;
  else {
    if ((r = buf_read_1(buf, "-")) < 0)
      return r;
    if (r) {
      negative = true;
      result += r;
    }
  }
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    return r;
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
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto ok;
  result += r;
  if ((r = pdf_buf_parse_comments(buf)) < 0)
    goto ok;
  if (! r)
    return -1;
  result += r;
 ok:
  if (negative)
    tmp = -tmp;
  *dest = tmp;
  return result;
}

sw pdf_buf_parse_integer (s_buf *buf, s32 *dest)
{
  bool negative = false;
  sw r;
  sw result = 0;
  s32 tmp;
  if ((r = buf_read_1(buf, "+")) < 0)
    return r;
  if (r)
    result += r;
  else {
    if ((r = buf_read_1(buf, "-")) < 0)
      return r;
    if (r) {
      negative = true;
      result += r;
    }
  }
  if ((r = buf_parse_s32_decimal(buf, negative, &tmp)) <= 0)
    return r;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto ok;
  result += r;
  if ((r = pdf_buf_parse_comments(buf)) < 0)
    goto ok;
  if (! r)
    return -1;
  result += r;
 ok:
  *dest = tmp;
  return result;
}

sw pdf_buf_parse_string (s_buf *buf, s_tag *dest)
{
  sw r;
  s_tag tmp = {0};
  if ((r = pdf_buf_parse_string_paren(buf, &tmp)) > 0) {
    *dest = tmp;
    return r;
  }
  return 0;
}

// parentheses must be balanced or backslashed
sw pdf_buf_parse_string_paren (s_buf *buf, s_tag *dest)
{
  char a[BUF_SIZE + 1] = {0};
  character c;
  character c1;
  character c2;
  s32 paren = 0;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_tag tmp = {0};
  s_buf tmp_buf = {0};
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  paren++;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      break;
    result += r;
    if (c == ')') {
      result += r;
      paren--;
      if (! paren)
        break;
    }
    else if (c == '\\') {
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        break;
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
        continue;
      }
    }
    if ((r = buf_write_character_utf8(&tmp_buf, c)) <= 0)
      break;
  }
  if (r < 0)
    goto restore;
  tmp.type = TAG_STR;
  if ((r = buf_read_to_str(&tmp_buf, &tmp.data.str)) < 0)
    goto restore;
  *dest = tmp;
  r = result;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_clean(&tmp_buf);
  buf_save_clean(buf, &save);
  return r;
}
