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
#ifndef KC3_PDF_H
#define KC3_PDF_H

#include "types.h"

/* Observers. */
s_pdf_file * pdf_parse_file (s_str *path, s_pdf_file *dest);
s_tag *      pdf_parse_from_str (s_str *str, s_tag *dest);

#endif /* KC3_PDF_H */
