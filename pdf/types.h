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
#ifndef KC3_PDF_TYPES_H
#define KC3_PDF_TYPES_H

#include "../libkc3/types.h"

/* Struct types. */
typedef struct pdf_stream s_pdf_stream;

/* 1 */

struct pdf_stream {
  s_map dictionnary;
  s64 offset;
  s64 length;
};

#endif /* KC3_PDF_TYPES_H */
