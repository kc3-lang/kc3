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
/* Gen from buf_inspect_u.h.in BITS=64 bits=64 */
#ifndef LIBKC3_BUF_INSPECT_U64_H
#define LIBKC3_BUF_INSPECT_U64_H

#include "types.h"
#include "buf_inspect_u64_decimal.h"

sw buf_inspect_u64 (s_buf *buf, u64 u);
sw buf_inspect_u64_base (s_buf *buf, const s_str *base, u64 u);
sw buf_inspect_u64_base_size (s_pretty *pretty, const s_str *base,
                                  u64 u);
sw buf_inspect_u64_size (s_pretty *pretty, u64 u);

#endif /* LIBKC3_BUF_INSPECT_U64_H */
