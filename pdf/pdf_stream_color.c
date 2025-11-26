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
#include "pdf_stream_draw.h"
#include "pdf_write.h"

sw pdf_stream_color_set_color(s_buf *buf, s_array *color, bool sb)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(buf, color)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, sb ? " CS" : " cs", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_set_grayscale(s_buf *buf, u8 gray_level, bool sb)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(buf, (s32)gray_level)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, sb ? " G" : " g", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_set_rgb(s_buf *buf, s_array *color, bool sb)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(buf, color)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, sb ? " RG" : " rg", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_set_cmyk(s_buf *buf, s_array *color, bool sb)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(buf, color)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, sb ? " K" : " k", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_use_shading(s_buf *buf, p_sym resource)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_name(buf, resource)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " sh", true)) < 0)
    return r;
  result += r;
  return result;
}
