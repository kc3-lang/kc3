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
/* Gen from buf_inspect_u_base.h.in BITS=64 bits=64 BASE=octal */
#ifndef LIBKC3_BUF_INSPECT_U64_octal_H
#define LIBKC3_BUF_INSPECT_U64_octal_H

#include "types.h"

sw buf_inspect_u64_octal (s_buf *buf, u64 x);
u8 buf_inspect_u64_octal_digits (u64 x);
sw buf_inspect_u64_octal_pad (s_buf *buf, u8 size, character pad,
                                   u64 x);
sw buf_inspect_u64_octal_size (s_pretty *pretty, u64 x);

#endif /* LIBKC3_BUF_INSPECT_U64_octal_H */
