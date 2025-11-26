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
typedef struct pdf_rect s_pdf_rect;
typedef struct pdf_vec2 s_pdf_vec2;
typedef struct pdf_catalog s_pdf_catalog;
typedef struct pdf_pages s_pdf_pages;
typedef struct pdf_page s_pdf_page;
typedef struct pdf_resources s_pdf_resources;
typedef struct pdf_font s_pdf_font;

/* Pointer types. */
typedef p_sym      p_pdf_name;
typedef p_sym_list p_pdf_name_list;

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

struct pdf_vec2 {
  f32 x;
  f32 y;
};

struct pdf_rect {
  f32 x;
  f32 y;
  f32 w;
  f32 h;
};

enum pdf_line_cap {
  PDF_LINE_CAP_BUTT   = 0,
  PDF_LINE_CAP_ROUND  = 1,
  PDF_LINE_CAP_SQUARE = 2,
};

enum pdf_line_join {
  PDF_LINE_JOIN_MITER = 0,
  PDF_LINE_JOIN_ROUND = 1,
  PDF_LINE_JOIN_BEVEL = 2,
};

#define PDF_FILL(even_odd) (((even_odd) << 1) | 1)

enum pdf_fill_mode {
  PDF_FILL_NONE     = 0,
  PDF_FILL_NZWR     = PDF_FILL(false),
  PDF_FILL_EVEN_ODD = PDF_FILL(true)
};


/* 2 */

struct pdf_pages {
  uw page_count;
  s_pdf_pages *pages;
};

struct pdf_catalog {
  s_pdf_pages pages;
};

struct pdf_file {
  s_pdf_catalog   body;
  s_str           header;
  p_pdf_name_list name_list;
  s_pdf_trailer   trailer;
  s_map           xref;
};

struct pdf_resources {
  s_map fonts; // map of name:pdf_font
};

struct pdf_font {
  p_sym subtype;
  p_sym base_font;
};

struct pdf_page {
  s_pdf_resources resources;
  s_pdf_rect page_box;
  s_tuple content_ref;
};

#endif /* KC3_PDF_TYPES_H */
