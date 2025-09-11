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
#include "pdf_file.h"
#include "pdf_trailer.h"

s_pdf_file * pdf_file_init (s_pdf_file *pdf_file)
{
  s_pdf_file tmp = {0};
  *pdf_file = tmp;
  return pdf_file;
}

void pdf_file_clean (s_pdf_file *pdf_file)
{
  pdf_trailer_clean(&pdf_file->trailer);
  str_clean(&pdf_file->header);
  do_block_clean(&pdf_file->body);
  map_clean(&pdf_file->xref);
}
