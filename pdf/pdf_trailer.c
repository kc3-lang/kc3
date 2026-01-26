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
#include "../libkc3/kc3.h"
#include "pdf_trailer.h"

s_pdf_trailer * pdf_trailer_init (s_pdf_trailer *trailer)
{
  s_pdf_trailer tmp = {0};
  *trailer = tmp;
  return trailer;
}

void pdf_trailer_clean (s_pdf_trailer *trailer)
{
  map_clean(&trailer->dictionnary);
}
