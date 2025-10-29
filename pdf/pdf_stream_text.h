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
#ifndef KC3_PDF_TEXT_H
#define KC3_PDF_TEXT_H

#include "types.h"

sw pdf_stream_text_begin (s_buf *buf);
sw pdf_stream_text_set_font (s_buf *buf, p_sym font, s32 size);
sw pdf_stream_text_set_pos_xy (s_buf *buf, s32 x, s32 y);
sw pdf_stream_text_set_text (s_buf *buf, s_str *text);
sw pdf_stream_text_set_text_array (s_buf *buf,
                                   const p_list text_array);
sw pdf_stream_text_end (s_buf *buf);

#endif /* KC3_PDF_TEXT_H */
