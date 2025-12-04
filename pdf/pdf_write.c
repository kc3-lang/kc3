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
#include "pdf_write.h"

void pdf_write_clean (s_pdf_write *pdf)
{
  str_clean(pdf->path);
  buf_delete(pdf->buf);
}

s_pdf_write * pdf_write_close (s_pdf_write *pdf)
{
  assert(pdf);
  assert(pdf->buf);
  buf_file_close(pdf->buf);
}

void pdf_write_delete (s_pdf_write *pdf)
{
  pdf_write_clean(pdf);
  free(pdf);
}

s_pdf_write * pdf_write_init (s_pdf_write *pdf)
{
  s_pdf_write tmp = {0};
  if (! (tmp.buf = buf_new_alloc()))
    return NULL;
  *pdf = tmp;
  return pdf;
}

s_pdf_write * pdf_write_init_file (s_pdf_write *pdf, s_str *path)
{
  s_pdf_write tmp = {0};
  if (! (tmp.buf = buf_new_alloc()))
    return NULL;
  if (! buf_file_open_r(tmp.buf, path)) {
    buf_delete(tmp.buf);
    return NULL;
  }
  *pdf = tmp;
  return pdf;
}

s_pdf_write * pdf_write_new (void)
{
  s_pdf_write *pdf;
  if (! (pdf = alloc(sizeof(s_pdf_write))))
    return NULL;
  if (! pdf_write_init(pdf)) {
    free(pdf);
    return NULL;
  }
  return pdf;
}

s_pdf_write * pdf_write_new_file (s_str *path)
{
  s_pdf_write *pdf;
  assert(path);
  if (! (pdf = alloc(sizeof(s_pdf_write))))
    return NULL;
  if (! pdf_write_init_file(pdf, path)) {
    free(pdf);
    return NULL;
  }
  return pdf;
}

s_str * pdf_write_to_str (s_pdf_write *pdf, s_str *dest)
{
  assert(pdf);
  assert(dest);
}
