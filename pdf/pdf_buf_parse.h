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
#ifndef KC3_PDF_H
#define KC3_PDF_H

#include "../libkc3/types.h"

sw pdf_buf_parse (s_buf *buf, s_tag *dest);
sw pdf_buf_parse_bool (s_buf *buf, bool *dest);
sw pdf_buf_parse_comment (s_buf *buf);
sw pdf_buf_parse_comments (s_buf *buf);
sw pdf_buf_parse_string (s_buf *buf, s_tag *dest);
sw pdf_buf_parse_string_paren (s_buf *buf, s_tag *dest);

#endif /* KC3_PDF_H */
