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

sw pdf_stream_text_begin (s_pdf_stream *stream)
{
  assert(stream);
  assert(stream->buf);
  return pdf_buf_write_token_clean(stream->buf, "BT", true);
}

sw pdf_stream_text_set_font (s_pdf_stream *stream,
                             p_sym font, s32 size)
{
  sw r = 0;
  sw result = 0;
  assert(stream);
  assert(stream->buf);
  if ((r = pdf_buf_write_name(stream->buf, font)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_integer(stream->buf, size)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0) {
    return r;
  }
  result += r;
  return result + pdf_buf_write_token_clean(stream->buf, "Tf", true);
}

sw pdf_stream_text_set_pos_xy (s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  sw r = 0;
  sw result = 0;
  assert(stream);
  assert(stream->buf);
  assert(pos);
  if ((r = pdf_buf_write_integer(stream->buf, pos->x)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_integer(stream->buf, pos->y)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0) {
    return r;
  }
  result += r;
  return result + pdf_buf_write_token_clean(stream->buf, "Td", true);
}

// REVIEW NOTES:
// The lack of separator between the string and "Tj" is normal here
// as the tokens can't collide (because of the string's bounding
// characters)
sw pdf_stream_text_set_text (s_pdf_stream *stream, s_str *text)
{
  sw r = 0;
  assert(stream);
  assert(stream->buf);
  if ((r = pdf_buf_write_string_hex(stream->buf, text)) < 0) {
    return r;
  }
  return r + pdf_buf_write_token_clean(stream->buf, "Tj", true);
}

// REVIEW NOTES:
// This expects a list of only integers and strings, where each integer
// indicates the horizontal (x) displacement of the next string, for
// things like kerning. This is TJ, while the simple string is Tj.
sw pdf_stream_text_set_text_array (s_pdf_stream *stream,
                                   const p_list text_array)
{
  sw r = 0;
  assert(stream);
  assert(stream->buf);
  if ((r = pdf_buf_write_array(stream->buf, text_array)) < 0) {
    return r;
  }
  return r + pdf_buf_write_token_clean(stream->buf, "TJ", true);
}

sw pdf_stream_text_end (s_pdf_stream *stream)
{
  assert(stream);
  assert(stream->buf);
  return pdf_buf_write_token_clean(stream->buf, "BT", true);
}
