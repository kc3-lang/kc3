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
#ifndef KC3_PDF_BUF_PARSE_H
#define KC3_PDF_BUF_PARSE_H

#include "types.h"

/* Parser functions. */
sw   pdf_buf_ignore_until_token (s_buf *buf, const char *token);
sw   pdf_buf_parse_array (s_buf *buf, s_pdf_file *pdf_file,
                          p_list *dest);
sw   pdf_buf_parse_bool (s_buf *buf, bool *dest);
sw   pdf_buf_parse_comment (s_buf *buf);
sw   pdf_buf_parse_comments (s_buf *buf);
sw   pdf_buf_parse_dictionnary (s_buf *buf, s_pdf_file *pdf_file,
                                s_map *dest);
sw   pdf_buf_parse_file (s_buf *buf, s_pdf_file *dest);
sw   pdf_buf_parse_file_body (s_buf *buf, s_pdf_file *pdf_file);
sw   pdf_buf_parse_file_header (s_buf *buf, s_str *dest);
sw   pdf_buf_parse_float (s_buf *buf, f64 *dest);
sw   pdf_buf_parse_indirect_object (s_buf *buf,
                                    s_pdf_file *pdf_file,
                                    s_tuple *dest);
sw   pdf_buf_parse_integer (s_buf *buf, s_tag *dest);
sw   pdf_buf_parse_name (s_buf *buf, s_pdf_file *pdf_file,
                         p_pdf_name *dest);
sw   pdf_buf_parse_null (s_buf *buf, s_tag *dest);
sw   pdf_buf_parse_number (s_buf *buf, s_tag *dest);
sw   pdf_buf_parse_object (s_buf *buf, s_pdf_file *pdf_file,
                           s_tag *dest);
sw   pdf_buf_parse_object_end (s_buf *buf, bool *end);
sw   pdf_buf_parse_rewind_to_trailer (s_buf *buf);
sw   pdf_buf_parse_stream (s_buf *buf, s_pdf_file *pdf_file,
                           s_tag *dest);
sw   pdf_buf_parse_string (s_buf *buf, s_tag *dest);
sw   pdf_buf_parse_string_hex (s_buf *buf, s_str *dest);
sw   pdf_buf_parse_string_paren (s_buf *buf, s_str *dest);
sw   pdf_buf_parse_token (s_buf *buf, const char *token);
sw   pdf_buf_parse_trailer (s_buf *buf, s_pdf_file *pdf_file,
                            s_pdf_trailer *dest);
sw   pdf_buf_parse_xref (s_buf *buf, s_map *dest);
bool pdf_character_is_delimiter (character c);

#endif /* KC3_PDF_BUF_PARSE_H */
