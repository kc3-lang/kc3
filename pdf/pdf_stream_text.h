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

sw pdf_stream_text_begin (s_pdf_stream *stream);
sw pdf_stream_text_set_font (s_pdf_stream *stream,
                             p_sym font, s32 size);
sw pdf_stream_text_set_pos_xy (s_pdf_stream *stream, s_pdf_vec2 *pos);
sw pdf_stream_text_set_text (s_pdf_stream *stream, s_str *text);
sw pdf_stream_text_set_text_array (s_pdf_stream *stream,
                                   const p_list text_array);
sw pdf_stream_text_end (s_pdf_stream *stream);

#endif /* KC3_PDF_TEXT_H */
