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
#include "../libkc3/kc3.h"
#include "pdf.h"
#include "pdf_buf_write.h"
#include "pdf_write.h"

void pdf_write_clean (s_pdf_write *pdf)
{
  str_clean(&pdf->path);
  buf_delete(pdf->buf);
  list_delete_all(pdf->fonts);
  list_delete_all(pdf->images);
  list_delete_all(pdf->pages);
}

void pdf_write_close (s_pdf_write *pdf)
{
  s_buf *buf;
  s_list *l;
  uw pages_count;
  uw fonts_count;
  uw images_count;
  uw i;
  assert(pdf);
  assert(pdf->buf);
  buf = pdf->buf;
  /* Count pages. */
  pages_count = list_length(pdf->pages);
  fonts_count = list_length(pdf->fonts);
  images_count = list_length(pdf->images);
  /* Write Catalog (object 1). */
  buf_write_1(buf, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");
  /* Write Pages (object 2). */
  buf_write_1(buf, "2 0 obj\n<< /Type /Pages /Kids [");
  l = pdf->pages;
  while (l) {
    buf_inspect_u32_decimal(buf, l->tag.data.u32);
    buf_write_1(buf, " 0 R");
    l = list_next(l);
    if (l)
      buf_write_1(buf, " ");
  }
  buf_write_1(buf, "] /Count ");
  buf_inspect_uw_decimal(buf, pages_count);
  buf_write_1(buf, " >>\nendobj\n");
  /* Write Resources (object 3). */
  buf_write_1(buf, "3 0 obj\n<<");
  if (fonts_count > 0) {
    buf_write_1(buf, " /Font <<");
    l = pdf->fonts;
    i = 1;
    while (l) {
      buf_write_1(buf, " /F");
      buf_inspect_uw_decimal(buf, i);
      buf_write_1(buf, " ");
      buf_inspect_u32_decimal(buf, l->tag.data.u32);
      buf_write_1(buf, " 0 R");
      l = list_next(l);
      i++;
    }
    buf_write_1(buf, " >>");
  }
  if (images_count > 0) {
    buf_write_1(buf, " /XObject <<");
    l = pdf->images;
    i = 1;
    while (l) {
      buf_write_1(buf, " /Im");
      buf_inspect_uw_decimal(buf, i);
      buf_write_1(buf, " ");
      buf_inspect_u32_decimal(buf, l->tag.data.u32);
      buf_write_1(buf, " 0 R");
      l = list_next(l);
      i++;
    }
    buf_write_1(buf, " >>");
  }
  buf_write_1(buf, " >>\nendobj\n");
  /* Write xref and trailer. */
  buf_write_1(buf, "xref\n0 1\n0000000000 65535 f \n");
  buf_write_1(buf, "trailer\n<< /Size ");
  buf_inspect_u32_decimal(buf, pdf->next_object_number);
  buf_write_1(buf, " /Root 1 0 R >>\nstartxref\n0\n%%EOF\n");
  buf_file_close(buf);
}

void pdf_write_delete (s_pdf_write *pdf)
{
  pdf_write_clean(pdf);
  free(pdf);
}

s_pdf_write * pdf_write_indirect_object (s_pdf_write *pdf,
                                         u32 object_number,
                                         u16 generation)
{
  u64 offset = 0;
  buf_tell_w(pdf->buf, &offset);
  buf_inspect_u32_decimal(pdf->buf, object_number);
  buf_write_1(pdf->buf, " ");
  buf_inspect_u16_decimal(pdf->buf, generation);
  buf_write_1(pdf->buf, " obj\n");
  return pdf;
}

s_pdf_write * pdf_write_init (s_pdf_write *pdf)
{
  s_pdf_write tmp = {0};
  if (! (tmp.buf = buf_new_alloc(BUF_SIZE)))
    return NULL;
  tmp.next_object_number = PDF_OBJECT_NUMBER_FIRST;
  *pdf = tmp;
  return pdf;
}

s_pdf_write * pdf_write_init_file (s_pdf_write *pdf, s_str *path)
{
  FILE *fp = NULL;
  s_pdf_write tmp = {0};
  if (! (tmp.buf = buf_new_alloc(BUF_SIZE)))
    return NULL;
  if (! (fp = file_open(path, "wb"))) {
    buf_delete(tmp.buf);
    return NULL;
  }
  if (! buf_file_open_w(tmp.buf, fp)) {
    fclose(fp);
    buf_delete(tmp.buf);
    return NULL;
  }
  tmp.next_object_number = PDF_OBJECT_NUMBER_FIRST;
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
  if (! pdf_buf_write_header(pdf->buf)) {
    free(pdf);
    return NULL;
  }
  return pdf;
}

u32 pdf_write_object_number_register (s_pdf_write *pdf)
{
  u32 object_number;
#if HAVE_PTHREAD
  mutex_lock(pdf->mutex);
#endif
  object_number = pdf->next_object_number;
  pdf->next_object_number++;
#if HAVE_PTHREAD
  mutex_unlock(pdf->mutex);
#endif
  return object_number;
}

s_str * pdf_write_to_str (s_pdf_write *pdf, s_str *dest)
{
  assert(pdf);
  assert(dest);
  if (buf_read_to_str(pdf->buf, dest) <= 0)
    return NULL;
  return dest;
}
