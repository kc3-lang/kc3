/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
/* Gen from buf_parse_s.h.in BITS=W bits=w */
#ifndef LIBC3_BUF_PARSE_SW_H
#define LIBC3_BUF_PARSE_SW_H

#include "types.h"

sw buf_parse_sw (s_buf *buf, sw *dest);
sw buf_parse_sw_base (s_buf *buf, const s_str *base,
                           bool negative, sw *dest);

#endif /* LIBC3_BUF_PARSE_SW_H */
