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

#ifndef KC3_PDF_STREAM_H
#define KC3_PDF_STREAM_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_pdf_stream *pdf_stream_init (s_pdf_stream *stream);
void pdf_stream_clean (s_pdf_stream *stream);

/* Heap allocation compatible functions. */
p_pdf_stream *pdf_stream_new (p_pdf_stream *dest);

#endif /* KC3_PDF_STREAM_H */
