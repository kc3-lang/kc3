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
#include "types.h"
#include <assert.h>
#include "pdf_stream.h"
#include "pdf_write.h"

void pdf_stream_clean (s_pdf_stream *stream)
{
  map_clean(&stream->dictionnary);
  buf_clean(stream->buf);
}

sw pdf_stream_color_set_cmyk(s_pdf_stream *stream,
                             s_array *color, bool sb)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(stream->buf, color)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf,
                                     sb ? " K" : " k", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_set_color(s_pdf_stream *stream,
                              s_array *color, bool sb)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(stream->buf, color)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf,
                                     sb ? " CS" : " cs", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_set_grayscale(s_pdf_stream *stream,
                                  u8 gray_level, bool sb)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(stream->buf, (s32)gray_level)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf,
                                     sb ? " G" : " g", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_set_rgb(s_pdf_stream *stream,
                            s_array *color, bool sb)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(stream->buf, color)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf,
                                     sb ? " RG" : " rg", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_color_use_shading(s_pdf_stream *stream, p_sym resource)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_name(stream->buf, resource)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " sh", true)) < 0)
    return r;
  result += r;
  return result;
}

void pdf_stream_delete (p_pdf_stream *stream)
{
  if (! stream || ! *stream)
    return;
  pdf_stream_clean(*stream);
  free(*stream);
  *stream = NULL;
}

sw pdf_stream_draw_clip(s_pdf_stream *stream, bool even_odd)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_token_clean(stream->buf,
      even_odd ? "W*" : "W", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_paint(s_pdf_stream *stream,
                         bool stroke,
                         enum pdf_fill_mode fill)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if (fill != PDF_FILL_NONE) {
  assert(stream);
  assert(stream->buf);
    if ((r = pdf_buf_write_token_clean(stream->buf,
        (fill == PDF_FILL_EVEN_ODD) ? "f*" : "f", true)) < 0)
      return r;
    result += r;
  }
  if (stroke) {
  assert(stream);
  assert(stream->buf);
    if ((r = pdf_buf_write_token_clean(stream->buf, "S", true)) < 0)
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
  assert(stream->buf);
  sw r;
  sw result = 0;
  if (fill != PDF_FILL_NONE) {
    if ((r = pdf_buf_write_token_clean(stream->buf,
        (fill == PDF_FILL_EVEN_ODD) ? "B*" : "B", true)) < 0)
      return r;
    result += r;
  }
  if (stroke) {
    if ((r = pdf_buf_write_token_clean(stream->buf, "b", true)) < 0)
      return r;
    result += r;
  }
  return result;
}

sw pdf_stream_draw_path_begin_at(s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  assert(stream);
  assert(stream->buf);
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(stream->buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " m", true)) < 0)
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
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(stream->buf, control1)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(stream->buf, control2)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(stream->buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " c", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic_v(s_pdf_stream *stream,
                                         s_pdf_vec2 *control2,
                                         s_pdf_vec2 *end)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(stream->buf, control2)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(stream->buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " v", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_curve_to_cubic_y(s_pdf_stream *stream,
                                         s_pdf_vec2 *control1,
                                         s_pdf_vec2 *end)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(stream->buf, control1)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_vec2(stream->buf, end)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " y", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_end_at(s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  assert(stream);
  assert(stream->buf);
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(stream->buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " v", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_move_to(s_pdf_stream *stream, s_pdf_vec2 *pos)
{
  assert(stream);
  assert(stream->buf);
  assert(pos);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_vec2(stream->buf, pos)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " l", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_path_rect(s_pdf_stream *stream, s_pdf_rect *rect)
{
  assert(stream);
  assert(stream->buf);
  assert(rect);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(stream->buf, rect->x)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(stream->buf, rect->y)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(stream->buf, rect->w)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(stream->buf, rect->h)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " re", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_cap(s_pdf_stream *stream,
                                enum pdf_line_cap cap)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(stream->buf, (s32)cap)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " J", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_dash_pattern(s_pdf_stream *stream,
                                         s_array *array,
                                         s32 phase)
{
  assert(stream);
  assert(stream->buf);
  assert(array);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_token(stream->buf, "[")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_flat_array(stream->buf, array)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token(stream->buf, "]")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(stream->buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_integer(stream->buf, phase)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " d", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_join(s_pdf_stream *stream,
                                 enum pdf_line_join join)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_integer(stream->buf, (s32)join)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " j", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_miter_limit(s_pdf_stream *stream,
                                        f32 limit)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(stream->buf, limit)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " M", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_line_width(s_pdf_stream *stream, f32 width)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(stream->buf, width)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " w", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_set_transformation_matrix(s_pdf_stream *stream,
                                             s_array *m)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_flat_array(stream->buf, m)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " cm", true)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_stream_draw_stack_load(s_pdf_stream *stream)
{
  assert(stream);
  assert(stream->buf);
  return pdf_buf_write_token_clean(stream->buf, "Q", true);
}

sw pdf_stream_draw_stack_save(s_pdf_stream *stream)
{
  assert(stream);
  assert(stream->buf);
  return pdf_buf_write_token_clean(stream->buf, "q", true);
}

sw pdf_stream_external_object(s_pdf_stream *stream, p_sym name)
{
  assert(stream);
  assert(stream->buf);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_name(stream->buf, name)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_token_clean(stream->buf, " Do", true)) < 0)
    return r;
  result += r;
  return result;
}

void pdf_stream_image(void) {}

s_pdf_stream * pdf_stream_init (s_pdf_stream *stream)
{
  s_pdf_stream tmp = {0};
  s_buf *buf;
  if (! stream)
    return NULL;
  if (! (buf = buf_new_alloc(BUF_SIZE)))
    return NULL;
  tmp.buf = buf;
  *stream = tmp;
  return stream;
}

p_pdf_stream * pdf_stream_new (p_pdf_stream *dest)
{
  p_pdf_stream tmp;
  if (! (tmp = alloc(sizeof(s_pdf_stream))))
    return NULL;
  if (! pdf_stream_init(tmp))
    return NULL;
  *dest = tmp;
  return dest;
}

sw pdf_stream_text_begin (s_pdf_stream *stream)
{
  assert(stream);
  assert(stream->buf);
  return pdf_buf_write_token_clean(stream->buf, "BT", true);
}

sw pdf_stream_text_end (s_pdf_stream *stream)
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
