/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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

#ifndef KC3_PDF_STREAM_H
#define KC3_PDF_STREAM_H

#include "../gl/types.h"
#include "types.h"

/* Stack-allocation compatible functions, call pdf_stream_clean
   after use. */
void           pdf_stream_clean (s_pdf_stream *stream);
s_pdf_stream * pdf_stream_init (s_pdf_stream *stream);

/* Heap allocation compatible functions, call pdf_stream_delete
   after use. */
void           pdf_stream_delete (p_pdf_stream *dest);
p_pdf_stream * pdf_stream_new (p_pdf_stream *dest);

/* Color operations */
sw pdf_stream_color_set_cmyk(s_pdf_stream *stream,
                             s_array *color, bool sb);
sw pdf_stream_color_set_color(s_pdf_stream *stream,
                              s_array *color, bool sb);
sw pdf_stream_color_set_grayscale(s_pdf_stream *stream,
                                  u8 gray_level, bool sb);
sw pdf_stream_color_set_rgb(s_pdf_stream *stream,
                            s_array *color, bool sb);
sw pdf_stream_color_use_shading(s_pdf_stream *stream,
                                p_sym resource);

/* Draw operations */
sw pdf_stream_draw_clip(s_pdf_stream *stream, bool even_odd);
sw pdf_stream_draw_paint(s_pdf_stream *stream,
                         bool stroke,
                         enum pdf_fill_mode fill);
sw pdf_stream_draw_paint_close(s_pdf_stream *stream,
                               bool stroke,
                               enum pdf_fill_mode fill);
sw pdf_stream_draw_path_begin_at(s_pdf_stream *stream,
                                 s_vec2 *pos);
sw pdf_stream_draw_path_curve_to_cubic(s_pdf_stream *stream,
                                       s_vec2 *control1,
                                       s_vec2 *control2,
                                       s_vec2 *end);
sw pdf_stream_draw_path_curve_to_cubic_v(s_pdf_stream *stream,
                                         s_vec2 *control2,
                                         s_vec2 *end);
sw pdf_stream_draw_path_curve_to_cubic_y(s_pdf_stream *stream,
                                         s_vec2 *control1,
                                         s_vec2 *end);
sw pdf_stream_draw_path_end_at(s_pdf_stream *stream, s_vec2 *pos);
sw pdf_stream_draw_path_move_to(s_pdf_stream *stream, s_vec2 *pos);
sw pdf_stream_draw_path_rect(s_pdf_stream *stream, s_pdf_rect *rect);
sw pdf_stream_draw_set_line_cap(s_pdf_stream *stream,
                                enum pdf_line_cap cap);
sw pdf_stream_draw_set_line_dash_pattern(s_pdf_stream *stream,
                                         s_array *array,
                                         s32 phase);
sw pdf_stream_draw_set_line_join(s_pdf_stream *stream,
                                 enum pdf_line_join join);
sw pdf_stream_draw_set_line_miter_limit(s_pdf_stream *stream,
                                        f32 limit);
sw pdf_stream_draw_set_line_width(s_pdf_stream *stream, f32 width);
sw pdf_stream_draw_set_transformation_matrix(s_pdf_stream *stream,
                                             s_array *m);
sw pdf_stream_draw_stack_load(s_pdf_stream *stream);
sw pdf_stream_draw_stack_save(s_pdf_stream *stream);
sw pdf_stream_external_object(s_pdf_stream *stream, p_sym name);

/* Image operations */
void pdf_stream_image(void);

/* Text operations */
sw pdf_stream_text_begin (s_pdf_stream *stream);
sw pdf_stream_text_end (s_pdf_stream *stream);
sw pdf_stream_text_set_font (s_pdf_stream *stream,
                             p_sym font, s32 size);
sw pdf_stream_text_set_pos_xy (s_pdf_stream *stream, s_vec2 *pos);
sw pdf_stream_text_set_text (s_pdf_stream *stream, s_str *text);
sw pdf_stream_text_set_text_array (s_pdf_stream *stream,
                                   const p_list text_array);

#endif /* KC3_PDF_STREAM_H */
