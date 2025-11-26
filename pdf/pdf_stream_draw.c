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
sw pdf_stream_draw_stack_save(s_pdf_stream *stream)
{
  assert(stream);
  return pdf_buf_write_token_clean(&stream->buf, "q", true);
}

sw pdf_stream_draw_stack_load(s_pdf_stream *stream)
{
  assert(stream);
  return pdf_buf_write_token_clean(&stream->buf, "Q", true);
}

// cm w J j M d
sw pdf_stream_draw_set_transformation_matrix(s_pdf_stream *stream,
                                             s_array *m)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(&stream->buf, m)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " cm", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_width(s_pdf_stream *stream, f32 width)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(&stream->buf, width)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " w", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_cap(s_pdf_stream *stream,
                                enum pdf_line_cap cap)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(&stream->buf, (s32)cap)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " J", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_join(s_pdf_stream *stream,
                                 enum pdf_line_join join)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(&stream->buf, (s32)join)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " j", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_miter_limit(s_pdf_stream *stream,
                                        f32 limit)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(&stream->buf, limit)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " M", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_dash_pattern(s_pdf_stream *stream,
                                         s_array *array,
                                         s32 phase)
{
  assert(stream);
  assert(array);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_token(&stream->buf, "[")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_flat_array(&stream->buf, array)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token(&stream->buf, "]")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_integer(&stream->buf, phase)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " d", true)) < 0)
    return r;
  result += r;
  return result;
}

// m l c v y h re
sw pdf_stream_draw_path_begin_at(s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  assert(stream);
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(&stream->buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " m", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_move_to(s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  assert(stream);
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(&stream->buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " l", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic(s_pdf_stream *stream, 
                                       s_pdf_vec2 *control1, 
                                       s_pdf_vec2 *control2, 
                                       s_pdf_vec2 *end)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(&stream->buf, control1)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(&stream->buf, control2)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(&stream->buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " c", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic_v(s_pdf_stream *stream, 
                                         s_pdf_vec2 *control2, 
                                         s_pdf_vec2 *end)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(&stream->buf, control2)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(&stream->buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " v", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic_y(s_pdf_stream *stream, 
                                         s_pdf_vec2 *control1, 
                                         s_pdf_vec2 *end)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(&stream->buf, control1)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(&stream->buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " y", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_end_at(s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  assert(stream);
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(&stream->buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " v", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_rect(s_pdf_stream *stream, s_pdf_rect *rect)
{
  assert(stream);
  assert(rect);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(&stream->buf, rect->x)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(&stream->buf, rect->y)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(&stream->buf, rect->w)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(&stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(&stream->buf, rect->h)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " re", true)) < 0)
    return r;
  result += r;
  return result;
}

// S s f f* B B* b b* n
sw pdf_stream_draw_paint(s_pdf_stream *stream,
                         bool stroke,
                         enum pdf_fill_mode fill)
{
  assert(stream);
  sw r;
  sw result = 0;
  if (fill != PDF_FILL_NONE) {
  assert(stream);
    if ((r = pdf_buf_write_token_clean(&stream->buf,
        (fill == PDF_FILL_EVEN_ODD) ? "f*" : "f", true)) < 0)
      return r;
    result += r;
  }
  if (stroke) {
  assert(stream);
    if ((r = pdf_buf_write_token_clean(&stream->buf, "S", true)) < 0)
      return r;
    result += r;
  }
  return result;
}

sw pdf_stream_draw_paint_close(s_pdf_stream *stream,
                               bool stroke,
                               enum pdf_fill_mode fill)
{
  assert(stream);
  sw r;
  sw result = 0;
  if (fill != PDF_FILL_NONE) {
  assert(stream);
    if ((r = pdf_buf_write_token_clean(&stream->buf,
        (fill == PDF_FILL_EVEN_ODD) ? "B*" : "B", true)) < 0)
      return r;
    result += r;
  }
  if (stroke) {
  assert(stream);
    if ((r = pdf_buf_write_token_clean(&stream->buf, "b", true)) < 0)
      return r;
    result += r;
  }
  return result;
}

// W W*
sw pdf_stream_draw_clip(s_pdf_stream *stream, bool even_odd)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_token_clean(&stream->buf,
      even_odd ? "W*" : "W", true)) < 0)
    return r;
  result += r;
  return result;
}

// Do
sw pdf_stream_external_object(s_pdf_stream *stream, p_sym name)
{
  assert(stream);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_name(&stream->buf, name)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(&stream->buf, " Do", true)) < 0)
    return r;
  result += r;
  return result;
}
