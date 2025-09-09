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
typedef struct pdf_file   s_pdf_file;
typedef struct pdf_stream s_pdf_stream;
typedef struct pdf_trailer s_pdf_trailer;

/* 1 */

struct pdf_stream {
  s_map dictionnary;
  u64 offset;
  u64 length;
};

struct pdf_trailer {
  s_map dictionnary;
  u64   startxref;
};

/* 2 */

struct pdf_file {
  s_pdf_trailer trailer;
  s_str         header;
  s_do_block    body;
  s_map         xref;
};

#endif /* KC3_PDF_TYPES_H */
