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

sw pdf_buf_parse_integer (s_buf *buf, s32 *dest)
{
  sw r;
  sw result = 0;
  s32 tmp;
  if ((r = buf_parse_s32_decimal(buf, &tmp)) <= 0)
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
