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

sw pdf_stream_draw_save_stack(s_buf *buf);
sw pdf_stream_draw_load_stack(s_buf *buf);

#endif /* KC3_PDF_STREAM_DRAW_H */
