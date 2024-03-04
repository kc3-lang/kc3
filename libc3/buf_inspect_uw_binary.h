/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
/* Gen from buf_inspect_u_base.h.in BITS=W bits=w BASE=binary */
#ifndef LIBC3_BUF_INSPECT_UW_binary_H
#define LIBC3_BUF_INSPECT_UW_binary_H

#include "types.h"

sw buf_inspect_uw_binary (s_buf *buf, const uw *u);
sw buf_inspect_uw_binary_size (const uw *u);

#endif /* LIBC3_BUF_INSPECT_UW_binary_H */
