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

enum pdf_line_cap {
  PDF_LINE_CAP_BUTT   = 0,
  PDF_LINE_CAP_ROUND  = 1,
  PDF_LINE_CAP_SQUARE = 2,
};

enum pdf_line_join {
  PDF_LINE_JOIN_MITER = 0,
  PDF_LINE_JOIN_ROUND = 1,
  PDF_LINE_JOIN_BEVEL = 2,
};

#define PDF_FILL(even_odd) (((even_odd) << 1) | 1)

enum pdf_fill_mode {
  PDF_FILL_NONE     = 0,
  PDF_FILL_NZWR     = PDF_FILL(false),
  PDF_FILL_EVEN_ODD = PDF_FILL(true)
};

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
sw pdf_stream_draw_path_begin_at(s_buf *buf, f32 x, f32 y);
sw pdf_stream_draw_path_move_to(s_buf *buf, f32 x, f32 y);
sw pdf_stream_draw_path_end_at(s_buf *buf, f32 x, f32 y);
sw pdf_stream_draw_path_rect(s_buf *buf, s_array *rect);

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
