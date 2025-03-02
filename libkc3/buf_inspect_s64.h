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
/* Gen from buf_inspect_s.h.in BITS=64 bits=64 */
#ifndef LIBKC3_BUF_INSPECT_S64_H
#define LIBKC3_BUF_INSPECT_S64_H

#include "types.h"
#include "buf_inspect_s64_decimal.h"

sw buf_inspect_s64 (s_buf *buf, const s64 *s);
sw buf_inspect_s64_size (s_pretty *pretty, const s64 *s);

#endif /* LIBKC3_BUF_INSPECT_S64_H */
