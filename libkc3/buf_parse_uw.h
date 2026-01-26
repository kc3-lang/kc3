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
/* Gen from buf_parse_u.h.in BITS=W bits=w */
#ifndef LIBKC3_BUF_PARSE_UW_H
#define LIBKC3_BUF_PARSE_UW_H

#include "types.h"

sw buf_parse_uw (s_buf *buf, uw *dest);
sw buf_parse_uw_base (s_buf *buf, const s_str *base,
                           uw *dest);
sw buf_parse_uw_decimal (s_buf *buf, uw *dest);
sw buf_parse_uw_hexadecimal (s_buf *buf, uw *dest);
s_tag * buf_parse_tag_uw (s_buf *buf, s_tag *dest);
s_tag * buf_parse_tag_uw_base (s_buf *buf, const s_str *base,
                                    s_tag *dest);

#endif /* LIBKC3_BUF_PARSE_UW_H */
