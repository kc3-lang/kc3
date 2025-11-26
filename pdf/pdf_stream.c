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
#include "pdf_stream.h"

s_pdf_stream * pdf_stream_init (s_pdf_stream *stream)
{
  s_pdf_stream tmp = {0};
  *stream = tmp;
  return stream;
}

void pdf_stream_clean (s_pdf_stream *stream)
{
  map_clean(&stream->dictionnary);
  buf_clean(stream->buf);
}
