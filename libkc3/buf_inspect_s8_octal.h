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
/* Gen from buf_inspect_s_base.c.in BITS=8 bits=8 base=octal */
#ifndef LIBKC3_BUF_INSPECT_S8_octal_H
#define LIBKC3_BUF_INSPECT_S8_octal_H

#include "types.h"

sw buf_inspect_s8_octal (s_buf *buf, s8 x);
sw buf_inspect_s8_octal_pad (s_buf *buf, u8 size, character pad,
                                   s8 x);
sw buf_inspect_s8_octal_size (s_pretty *pretty, s8 x);

#endif /* LIBKC3_BUF_INSPECT_S8_octal_H */
