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
  assert(buf);
  return pdf_buf_write_token_clean(buf, "BT", true);
  return pdf_buf_write_token_clean(buf, "BT", true);
  return pdf_buf_write_token_clean(buf, "BT", true);
}

sw pdf_stream_text_end (s_buf *buf)
{
  assert(buf);
  return pdf_buf_write_token_clean(buf, "BT", true);
}
