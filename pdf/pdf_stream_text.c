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
#include "../libkc3/kc3_main.h"
#include "pdf_write.h"
#include "pdf_stream_text.h"

sw pdf_stream_text_begin (s_buf *buf)
{
  assert(buf);
  return pdf_buf_write_token_clean(buf, "BT", true);
}

sw pdf_stream_text_set_font (s_buf *buf, p_sym font, s32 size)
{
  sw r = 0;
  assert(buf);
  if ((r += pdf_buf_write_name(buf, font)) < 0) {
    return r;
  }
  if ((r += pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  if ((r += pdf_buf_write_integer(buf, size)) < 0) {
    return r;
  }
  if ((r += pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  return pdf_buf_write_token_clean(buf, "Tf", true);
}

sw pdf_stream_text_set_pos_xy (s_buf *buf, s32 x, s32 y)
{
  sw r = 0;
  assert(buf);
  if ((r += pdf_buf_write_integer(buf, x)) < 0) {
    return r;
  }
  if ((r += pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  if ((r += pdf_buf_write_integer(buf, y)) < 0) {
    return r;
  }
  if ((r += pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  return pdf_buf_write_token_clean(buf, "Td", true);
}

sw pdf_stream_text_end (s_buf *buf)
{
  assert(buf);
  return pdf_buf_write_token_clean(buf, "BT", true);
}
