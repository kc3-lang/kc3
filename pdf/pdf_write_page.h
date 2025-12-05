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

s_pdf_write_page ** kc3_pdf_write_page_new (s_pdf_write_page **page,
                                            s_pdf_write **pdf,
                                            s_pdf_rect *box);

s_pdf_write_page * pdf_write_page_init (s_pdf_write_page *page,
                                        s_pdf_write *pdf,
                                        s_pdf_rect *box);

#endif /* KC3_PDF_WRITE_PAGE_H */
