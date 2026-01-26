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
#ifndef KC3_PDF_FILE_H
#define KC3_PDF_FILE_H

#include "types.h"

/* Stack-allocation compatible functions. */
void pdf_file_clean (s_pdf_file *pdf_file);

/* Observers. */
s_tag * pdf_file_get_indirect_object (s_pdf_file *pdf_file,
                                      s_tag *ref, s_tag *dest);

#endif /* KC3_PDF_FILE_H */
