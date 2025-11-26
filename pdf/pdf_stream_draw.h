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

#ifndef KC3_PDF_STREAM_DRAW_H
#define KC3_PDF_STREAM_DRAW_H

#include "types.h"

// q Q
sw pdf_stream_draw_stack_save(s_buf *buf);
sw pdf_stream_draw_stack_load(s_buf *buf);

// cm w J j M d
sw pdf_stream_draw_set_transformation_matrix(s_buf *buf, s_array *m);
sw pdf_stream_draw_set_line_width(s_buf *buf, f32 width);
sw pdf_stream_draw_set_line_cap(s_buf *buf, enum pdf_line_cap cap);
sw pdf_stream_draw_set_line_join(s_buf *buf, enum pdf_line_join join);
sw pdf_stream_draw_set_line_miter_limit(s_buf *buf, f32 limit);
sw pdf_stream_draw_set_line_dash_pattern(s_buf *buf,
                                         s_array *array,
                                         s32 phase);
// sw pdf_stream_draw_set_line_flat_tolerance(s_buf *buf,
//                                            f32 tolerance);

// m l c v y h re
sw pdf_stream_draw_path_begin_at(s_buf *buf, s_pdf_vec2 *pos);
sw pdf_stream_draw_path_move_to(s_buf *buf, s_pdf_vec2 *pos);
sw pdf_stream_draw_path_curve_to_cubic(s_buf *buf, 
                                       s_pdf_vec2 *control1, 
                                       s_pdf_vec2 *control2, 
                                       s_pdf_vec2 *end);
sw pdf_stream_draw_path_curve_to_cubic_v(s_buf *buf, 
                                         s_pdf_vec2 *control2, 
                                         s_pdf_vec2 *end);
sw pdf_stream_draw_path_curve_to_cubic_y(s_buf *buf, 
                                         s_pdf_vec2 *control1, 
                                         s_pdf_vec2 *end);  
sw pdf_stream_draw_path_end_at(s_buf *buf, s_pdf_vec2 *pos);
sw pdf_stream_draw_path_rect(s_buf *buf, s_pdf_rect *rect);

// S s f f* B B* b b* n
sw pdf_stream_draw_paint(s_buf *buf,
                         bool stroke,
                         enum pdf_fill_mode fill);
sw pdf_stream_draw_paint_close(s_buf *buf,
                               bool stroke,
                               enum pdf_fill_mode fill);

// W W*
sw pdf_stream_draw_clip(s_buf *buf, bool even_odd);

// Do
sw pdf_stream_external_object(s_buf *buf, p_sym name);

#endif /* KC3_PDF_STREAM_DRAW_H */
