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
#ifndef KC3_PDF_WRITE_H
#define KC3_PDF_WRITE_H

#include "types.h"

/* Stack-allocation compatible functions, call pdf_write_clean
   after use. */
void          pdf_write_clean (s_pdf_write *pdf);
s_pdf_write * pdf_write_init (s_pdf_write *pdf);
s_pdf_write * pdf_write_init_file (s_pdf_write *pdf, s_str *path);

/* Heap-allocation functions, call pdf_write_delete after use. */
void          pdf_write_delete (s_pdf_write *pdf);
s_pdf_write * pdf_write_new (void);
s_pdf_write * pdf_write_new_file (s_str *path);

/* Observers. */
s_str * pdf_write_to_str (s_pdf_write *pdf, s_str *dest);

/* Operators. */
void pdf_write_close (s_pdf_write *pdf);
u32  pdf_write_object_number_register (s_pdf_write *pdf);
s_pdf_write * pdf_write_indirect_object (s_pdf_write *pdf,
                                         u32 object_number,
                                         u16 generation);
 
#endif /* KC3_PDF_WRITE_H */
