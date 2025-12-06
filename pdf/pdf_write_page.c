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
  (void) page;
  (void) x;
  (void) y;
  (void) image;
  err_puts("kc3_pdf_write_page_image: TODO");
  return page;
}

s_pdf_write_page ** kc3_pdf_write_page_rectangle (s_pdf_write_page **page,
                                                  f32 x, f32 y,
                                                  f32 w, f32 h)
{
  (void) page;
  (void) x;
  (void) y;
  (void) w;
  (void) h;
  err_puts("kc3_pdf_write_page_rectangle: TODO");
  return page;
}

s_pdf_write_page **
kc3_pdf_write_page_set_color_rgb (s_pdf_write_page **page, f32 r, f32 g,
                                  f32 b)
{
  assert(page);
  assert(*page);
  (void) r;
  (void) g;
  (void) b;
  err_puts("kc3_pdf_write_page_set_color_rgb: TODO");
  return page;
}

s_pdf_write_page ** kc3_pdf_write_page_text (s_pdf_write_page **page,
                                             f32 x, f32 y, u32 font,
                                             f32 font_size, s_str *text)
{
  (void) page;
  (void) x;
  (void) y;
  (void) font;
  (void) font_size;
  (void) text;
  err_puts("kc3_pdf_write_page_text: TODO");
  return page;
}
