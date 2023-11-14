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
/* Gen from buf_inspect_u_base.h.in BITS=32 bits=32 BASE=binary */
#ifndef LIBC3_BUF_INSPECT_U32_binary_H
#define LIBC3_BUF_INSPECT_U32_binary_H

#include "types.h"

sw buf_inspect_u32_binary (s_buf *buf, const u32 *u);
sw buf_inspect_u32_binary_size (const u32 *u);

#endif /* LIBC3_BUF_INSPECT_U32_binary_H */
