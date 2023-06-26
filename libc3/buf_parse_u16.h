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
/* Gen from buf_parse_u.h.in BITS=16 bits=16 */
#ifndef BUF_PARSE_U16_H
#define BUF_PARSE_U16_H

#include "types.h"

sw buf_parse_u16 (s_buf *buf, u16 *dest);
sw buf_parse_u16_base (s_buf *buf, const s_str *base,
                           u16 *dest);

#endif /* #ifndef BUF_PARSE_U16_H */
