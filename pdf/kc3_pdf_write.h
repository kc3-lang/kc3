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
#ifndef KC3_KC3_PDF_WRITE_H
#define KC3_KC3_PDF_WRITE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "types.h"

/* Heap-allocation functions, call kc3_pdf_write_delete after use. */
void           kc3_pdf_write_delete (s_pdf_write **pdf);
s_pdf_write ** kc3_pdf_write_new_file (s_pdf_write **pdf, s_str *path);
s_pdf_write ** kc3_pdf_write_new (s_pdf_write **pdf);

/* Observers. */
s_str * kc3_pdf_write_to_str (s_pdf_write **pdf, s_str *dest);

/* Operators. */
void kc3_pdf_write_close (s_pdf_write **pdf);
u32  kc3_pdf_write_font_from_file (s_pdf_write **pdf, s_str *path);
u32  kc3_pdf_write_image_from_file (s_pdf_write **pdf, s_str *path);
sw   kc3_pdf_write_jpeg_xobject (s_buf *buf, u32 object_number,
                                 u32 width, u32 height, u32 components,
                                 FILE *fp, uw length);
sw   kc3_pdf_write_ttf_font (s_buf *buf, u32 font_obj,
                             u32 descriptor_obj, const char *font_name);
sw   kc3_pdf_write_ttf_descriptor (s_buf *buf, u32 descriptor_obj,
                                   u32 file_obj,
                                   const char *font_name,
                                   u16 units_per_em,
                                   s16 ascender, s16 descender,
                                   const FT_BBox *bbox,
                                   uw file_length);
sw   kc3_pdf_write_ttf_file (s_buf *buf, u32 file_obj, FILE *fp,
                             uw length);

#endif /* KC3_KC3_PDF_WRITE_H */
