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
typedef struct pdf_file                 s_pdf_file;
typedef struct pdf_rect                 s_pdf_rect;
typedef struct pdf_stream               s_pdf_stream;
typedef struct pdf_trailer              s_pdf_trailer;
typedef struct pdf_write                s_pdf_write;
typedef struct pdf_write_content_stream s_pdf_write_content_stream;
typedef struct pdf_write_page           s_pdf_write_page;
typedef struct pdf_write_stream         s_pdf_write_stream;

/* Pointer types. */
typedef p_sym              p_pdf_name;
typedef p_sym_list         p_pdf_name_list;
typedef struct pdf_stream *p_pdf_stream;

/* Enums. */
enum pdf_fill_mode {
  PDF_FILL_NONE     = 0,
  PDF_FILL_NZWR     = 1,
  PDF_FILL_EVEN_ODD = 3,
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

typedef enum pdf_object_number {
  PDF_OBJECT_NUMBER_CATALOG   = 1,
  PDF_OBJECT_NUMBER_PAGES     = 2,
  PDF_OBJECT_NUMBER_RESOURCES = 3,
  PDF_OBJECT_NUMBER_FIRST     = 4,
} e_pdf_object_number;


/* 1 */

struct pdf_rect {
  f32 x;
  f32 y;
  f32 w;
  f32 h;
};


struct pdf_stream {
  s_map  dictionnary;
  s64    offset;
  s64    length;
  s_buf *buf;
};

struct pdf_trailer {
  s_map dictionnary;
  u64   startxref;
};

struct pdf_write {
  s_str path;
  s_buf *buf;
  s_mutex *mutex;
  u32 next_object_number;
  s_list *pages;
  s_list *fonts;
  s_list *images;
  s_map xref;  
};

struct pdf_write_stream {
  u32 object_number;
  s_pdf_stream stream;
};

/* 2 */

struct pdf_file {
  s_map           body;
  s_str           header;
  p_pdf_name_list name_list;
  s_pdf_trailer   trailer;
  s_map           xref;
};

struct pdf_write_content_stream {
  s_pdf_write_stream stream;
};

struct pdf_write_page {
  s_map map;
  u32 object_number;
  s_pdf_write_content_stream contents;
};

#endif /* KC3_PDF_TYPES_H */
