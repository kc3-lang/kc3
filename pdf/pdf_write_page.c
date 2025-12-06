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
#include "pdf.h"
#include "pdf_buf_write.h"
#include "pdf_stream.h"
#include "pdf_write.h"
#include "pdf_write_page.h"

s_pdf_write_page ** kc3_pdf_write_page_new (s_pdf_write_page **page,
                                            s_pdf_write **pdf,
                                            s_pdf_rect *box)
{
  s_pdf_write_page *tmp;
  if (! (tmp = alloc(sizeof(s_pdf_write_page))))
    return NULL;
  if (! pdf_write_page_init(tmp, *pdf, box)) {
    free(tmp);
    return NULL;
  }
  *page = tmp;
  return page;
}

s_pdf_write_page * pdf_write_page_init (s_pdf_write_page *page,
                                        s_pdf_write *pdf,
                                        s_pdf_rect *box)
{
  u32 content_stream_object_number;
  s_pdf_write_page tmp = {0};
  if (! map_init(&tmp.map, 5))
    return NULL;
  tmp.object_number = pdf_write_object_number_register(pdf);
  content_stream_object_number = pdf_write_object_number_register(pdf);
  tmp.contents.stream.object_number = content_stream_object_number;
  if (! pdf_stream_init(&tmp.contents.stream.stream))
    return NULL;
  tag_init_psym(tmp.map.key,   sym_1("Type"));
  tag_init_psym(tmp.map.value, sym_1("Page"));
  tag_init_psym(tmp.map.key       + 1, sym_1("Parent"));
  pdf_tag_init_xref(tmp.map.value + 1, PDF_OBJECT_NUMBER_PAGES, 0);
  tag_init_psym(tmp.map.key       + 2, sym_1("Resources"));
  pdf_tag_init_xref(tmp.map.value + 2, PDF_OBJECT_NUMBER_RESOURCES, 0);
  tag_init_psym(tmp.map.key                + 3, sym_1("MediaBox"));
  tag_init_pstruct_copy_data(tmp.map.value + 3, sym_1("PDF.Rect"), box);
  tag_init_psym(tmp.map.key       + 4, sym_1("Contents"));
  pdf_tag_init_xref(tmp.map.value + 4, content_stream_object_number, 0);
  *page = tmp;
  return page;
}

s_pdf_write_page ** kc3_pdf_write_page_image (s_pdf_write_page **page,
                                              f32 x, f32 y, u32 image)
{
  s_buf *buf;
  assert(page);
  assert(*page);
  buf = (*page)->contents.stream.stream.buf;
  assert(buf);
  buf_write_1(buf, "q\n");
  pdf_buf_write_float(buf, x);
  buf_write_1(buf, " 0 0 ");
  pdf_buf_write_float(buf, y);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, x);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, y);
  buf_write_1(buf, " cm\n");
  buf_write_1(buf, "/Im");
  buf_inspect_u32(buf, image);
  buf_write_1(buf, " Do\n");
  buf_write_1(buf, "Q\n");
  return page;
}

s_pdf_write_page ** kc3_pdf_write_page_rectangle (s_pdf_write_page **page,
                                                  s_pdf_rect *rect)
{
  s_buf *buf;
  assert(page);
  assert(*page);
  assert(rect);
  buf = (*page)->contents.stream.stream.buf;
  assert(buf);
  pdf_buf_write_float(buf, rect->x);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, rect->y);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, rect->w);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, rect->h);
  buf_write_1(buf, " re\n");
  buf_write_1(buf, "S\n");
  return page;
}

s_pdf_write_page **
kc3_pdf_write_page_set_color_rgb (s_pdf_write_page **page, f32 r, f32 g,
                                  f32 b)
{
  s_buf *buf;
  assert(page);
  assert(*page);
  buf = (*page)->contents.stream.stream.buf;
  assert(buf);
  pdf_buf_write_float(buf, r);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, g);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, b);
  buf_write_1(buf, " rg\n");
  return page;
}

s_pdf_write_page ** kc3_pdf_write_page_text (s_pdf_write_page **page,
                                             f32 x, f32 y, u32 font,
                                             f32 font_size, s_str *text)
{
  s_buf *buf;
  assert(page);
  assert(*page);
  assert(text);
  buf = (*page)->contents.stream.stream.buf;
  assert(buf);
  buf_write_1(buf, "BT\n");
  buf_write_1(buf, "/F");
  buf_inspect_u32_decimal(buf, font);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, font_size);
  buf_write_1(buf, " Tf\n");
  pdf_buf_write_float(buf, x);
  buf_write_1(buf, " ");
  pdf_buf_write_float(buf, y);
  buf_write_1(buf, " Td\n");
  pdf_buf_write_string_hex(buf, text);
  buf_write_1(buf, " Tj\n");
  buf_write_1(buf, "ET\n");
  return page;
}
