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

sw pdf_buf_parse_comment (s_buf *buf)
{
  sw r;
  sw result = 0;
  if ((r = buf_parse_1(buf, "%")) <= 0)
    return r;
  result += r;
  if ((r = buf_read_until_1_into_str(buf, "\n")) < 0)
    return result;
  return result;
}

sw pdf_buf_parse_comments (s_buf *buf)
{
  sw r;
  sw result = 0;
  while ((r = pdf_buf_parse_comment(buf)) > 0)
    ;
  if (r < 0)
    return r;
  return result;
}
