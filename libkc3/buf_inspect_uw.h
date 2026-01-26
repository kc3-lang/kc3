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
/* Gen from buf_inspect_u.h.in BITS=W bits=w */
#ifndef LIBKC3_BUF_INSPECT_UW_H
#define LIBKC3_BUF_INSPECT_UW_H

#include "types.h"
#include "buf_inspect_uw_decimal.h"

sw buf_inspect_uw (s_buf *buf, uw u);
sw buf_inspect_uw_base (s_buf *buf, const s_str *base, uw u);
sw buf_inspect_uw_base_size (s_pretty *pretty, const s_str *base,
                                  uw u);
sw buf_inspect_uw_size (s_pretty *pretty, uw u);

#endif /* LIBKC3_BUF_INSPECT_UW_H */
