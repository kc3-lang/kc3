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
#ifndef KC3_PDF_WRITE_H
#define KC3_PDF_WRITE_H

#include "types.h"

/* Heap allocation functions, call pdf_write_delete after use. */
void          pdf_write_delete (s_pdf_write *pdf_write);
s_pdf_write * pdf_write_new_to_file (s_str *path);
s_pdf_write * pdf_write_new (void);

#endif /* KC3_PDF_WRITE_H */
