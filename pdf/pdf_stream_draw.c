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
#include <assert.h>
#include "pdf_stream_draw.h"
#include "pdf_write.h"

// q Q
sw pdf_stream_draw_stack_save(s_buf *buf)
{
  return pdf_buf_write_token_clean(buf, "q", true);
}

sw pdf_stream_draw_stack_load(s_buf *buf)
{
  return pdf_buf_write_token_clean(buf, "Q", true);
}

// cm w J j M d
sw pdf_stream_draw_set_transformation_matrix(s_buf *buf, s_array *m)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(buf, m)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " cm", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_width(s_buf *buf, f32 width)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(buf, width)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " w", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_cap(s_buf *buf, enum pdf_line_cap cap)
{
  assert(buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(buf, (s32)cap)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " J", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_join(s_buf *buf, enum pdf_line_join join)
{
  assert(buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(buf, (s32)join)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " j", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_miter_limit(s_buf *buf, f32 limit)
{
  assert(buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(buf, limit)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " M", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_dash_pattern(s_buf *buf,
                                         s_array *array,
                                         s32 phase)
{
  assert(buf);
  assert(array);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_token(buf, "[")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_flat_array(buf, array)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token(buf, "]")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_integer(buf, phase)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " d", true)) < 0)
    return r;
  result += r;
  return result;
}

// m l c v y h re
sw pdf_stream_draw_path_begin_at(s_buf *buf, s_pdf_vec2 *pos)
{
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " m", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_move_to(s_buf *buf, s_pdf_vec2 *pos)
{
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " l", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic(s_buf *buf, 
                                       s_pdf_vec2 *control1, 
                                       s_pdf_vec2 *control2, 
                                       s_pdf_vec2 *end)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(buf, control1)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(buf, control2)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " c", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic_v(s_buf *buf, 
                                         s_pdf_vec2 *control2, 
                                         s_pdf_vec2 *end)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(buf, control2)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " v", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic_y(s_buf *buf, 
                                         s_pdf_vec2 *control1, 
                                         s_pdf_vec2 *end)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(buf, control1)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " y", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_end_at(s_buf *buf, s_pdf_vec2 *pos)
{
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " v", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_rect(s_buf *buf, s_pdf_rect *rect)
{
  assert(rect);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(buf, rect->x)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->y)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->w)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->h)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " re", true)) < 0)
    return r;
  result += r;
  return result;
}

// S s f f* B B* b b* n
sw pdf_stream_draw_paint(s_buf *buf,
                         bool stroke,
                         enum pdf_fill_mode fill)
{
  sw r;
  sw result = 0;
  if (fill != PDF_FILL_NONE) {
    if ((r = pdf_buf_write_token_clean(buf,
        (fill == PDF_FILL_EVEN_ODD) ? "f*" : "f", true)) < 0)
      return r;
    result += r;
  }
  if (stroke) {
    if ((r = pdf_buf_write_token_clean(buf, "S", true)) < 0)
      return r;
    result += r;
  }
  return result;
}

sw pdf_stream_draw_paint_close(s_buf *buf,
                               bool stroke,
                               enum pdf_fill_mode fill)
{
  sw r;
  sw result = 0;
  if (fill != PDF_FILL_NONE) {
    if ((r = pdf_buf_write_token_clean(buf,
        (fill == PDF_FILL_EVEN_ODD) ? "B*" : "B", true)) < 0)
      return r;
    result += r;
  }
  if (stroke) {
    if ((r = pdf_buf_write_token_clean(buf, "b", true)) < 0)
      return r;
    result += r;
  }
  return result;
}

// W W*
sw pdf_stream_draw_clip(s_buf *buf, bool even_odd)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_token_clean(buf,
      even_odd ? "W*" : "W", true)) < 0)
    return r;
  result += r;
  return result;
}

// Do
sw pdf_stream_external_object(s_buf *buf, p_sym name)
{
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_name(buf, name)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(buf, " Do", true)) < 0)
    return r;
  result += r;
  return result;
}
