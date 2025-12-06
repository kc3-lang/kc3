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
#ifndef KC3_PDF_BUF_WRITE_H
#define KC3_PDF_BUF_WRITE_H

#include "../gl/types.h"
#include "types.h"

sw pdf_buf_write_array (s_buf *buf, const p_list src);
sw pdf_buf_write_bool (s_buf *buf, bool src);
sw pdf_buf_write_dictionnary (s_buf *buf, const s_map *src);
sw pdf_buf_write_flat_array (s_buf *buf, const s_array *src);
sw pdf_buf_write_float (s_buf *buf, f32 src);
sw pdf_buf_write_header (s_buf *buf);
sw pdf_buf_write_indirect_object (s_buf *buf, const s_tuple *src);
sw pdf_buf_write_integer (s_buf *buf, s32 src);
sw pdf_buf_write_name (s_buf *buf, p_sym src);
sw pdf_buf_write_null (s_buf *buf);
sw pdf_buf_write_separator(s_buf *buf, bool newline);
sw pdf_buf_write_string_hex (s_buf *buf, const s_str *src);
sw pdf_buf_write_tag (s_buf *buf, const s_tag *src);
sw pdf_buf_write_token (s_buf *buf, const char *token);
sw pdf_buf_write_token_with_separator (s_buf *buf,
                                       const char *pchar,
                                       bool newline);
sw pdf_buf_write_vec2 (s_buf *buf, const s_vec2 *src);

#endif /* KC3_PDF_BUF_WRITE_H */
