/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
/* Gen from buf_inspect_s_base.c.in BITS=64 bits=64 base=hexadecimal */
#ifndef LIBKC3_BUF_INSPECT_S64_hexadecimal_H
#define LIBKC3_BUF_INSPECT_S64_hexadecimal_H

#include "types.h"

sw buf_inspect_s64_hexadecimal (s_buf *buf, const s64 *s);
sw buf_inspect_s64_hexadecimal_size (const s64 *s);

#endif /* LIBKC3_BUF_INSPECT_S64_hexadecimal_H */
