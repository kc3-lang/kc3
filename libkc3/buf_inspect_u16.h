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
/* Gen from buf_inspect_u.h.in BITS=16 bits=16 */
#ifndef LIBKC3_BUF_INSPECT_U16_H
#define LIBKC3_BUF_INSPECT_U16_H

#include "types.h"
#include "buf_inspect_u16_decimal.h"

sw buf_inspect_u16 (s_buf *buf, u16 u);
sw buf_inspect_u16_base (s_buf *buf, const s_str *base, u16 u);
sw buf_inspect_u16_base_size (s_pretty *pretty, const s_str *base,
                                  u16 u);
sw buf_inspect_u16_size (s_pretty *pretty, u16 u);

#endif /* LIBKC3_BUF_INSPECT_U16_H */
