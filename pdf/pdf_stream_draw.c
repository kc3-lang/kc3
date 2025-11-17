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

// q Q
sw pdf_stream_draw_stack_save(s_buf *buf)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_stack_load(s_buf *buf)
{
  sw r;
  sw result = 0;
  return result;
}

// cm w J j M d
sw pdf_stream_draw_set_transformation_matrix(s_buf *buf, s_array *m)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_set_line_width(s_buf *buf, f32 width)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_set_line_cap(s_buf *buf, enum pdf_line_cap cap)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_set_line_join(s_buf *buf, enum pdf_line_join join)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_set_line_miter_limit(s_buf *buf, f32 limit)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_set_line_dash_pattern(s_buf *buf,
                                         s_array *array,
                                         s32 phase)
{
  sw r;
  sw result = 0;
  return result;
}

// m l c v y h re
sw pdf_stream_draw_path_begin_at(s_buf *buf, f32 x, f32 y)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_path_move_to(s_buf *buf, f32 x, f32 y)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_path_end_at(s_buf *buf, f32 x, f32 y)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_path_rect(s_buf *buf, s_array *rect)
{
  sw r;
  sw result = 0;
  return result;
}

// S s f f* B B* b b* n
sw pdf_stream_draw_paint(s_buf *buf,
                         bool stroke,
                         enum pdf_fill_mode fill)
{
  sw r;
  sw result = 0;
  return result;
}

sw pdf_stream_draw_paint_close(s_buf *buf,
                               bool stroke,
                               enum pdf_fill_mode fill)
{
  sw r;
  sw result = 0;
  return result;
}

// W W*
sw pdf_stream_draw_clip(s_buf *buf, bool even_odd)
{
  sw r;
  sw result = 0;
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
