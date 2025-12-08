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
#ifndef KC3_PDF_WRITE_PAGE_H
#define KC3_PDF_WRITE_PAGE_H

#include "types.h"

void pdf_write_page_clean (s_pdf_write_page *page);

/* Heap-allocation functions. */
s_pdf_write_page ** kc3_pdf_write_page_new (s_pdf_write_page **page,
                                            s_pdf_write **pdf,
                                            s_pdf_rect *box);

/* Operators. */
s_pdf_write_page **
kc3_pdf_write_page_image (s_pdf_write_page **page,
                          f32 x, f32 y, u32 image);
s_pdf_write_page **
kc3_pdf_write_page_rectangle (s_pdf_write_page **page,
                              s_pdf_rect *rect);
s_pdf_write_page **
kc3_pdf_write_page_set_color_rgb (s_pdf_write_page **page,
                                  f32 r, f32 g,
                                  f32 b);
s_pdf_write_page **
kc3_pdf_write_page_text (s_pdf_write_page **page,
                         f32 x, f32 y, u32 font,
                         f32 font_size, s_str *text);

/* Initializers. */
s_pdf_write_page * pdf_write_page_init (s_pdf_write_page *page,
                                        s_pdf_write *pdf,
                                        s_pdf_rect *box);

#endif /* KC3_PDF_WRITE_PAGE_H */
