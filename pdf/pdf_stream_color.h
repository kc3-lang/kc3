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

// cs sc scn G g RG rg K k
sw pdf_stream_color_set_color(s_buf *buf, s_array *color, bool sb);
sw pdf_stream_color_set_grayscale(s_buf *buf, u8 gray_level, bool sb);
sw pdf_stream_color_set_rgb(s_buf *buf, s_array *color, bool sb);
sw pdf_stream_color_set_cmyk(s_buf *buf, s_array *color, bool sb);
sw pdf_stream_color_use_shading(s_buf *buf, p_sym resource);

#endif /* KC3_PDF_STREAM_DRAW_H */
