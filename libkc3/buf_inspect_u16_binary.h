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
/* Gen from buf_inspect_u_base.h.in BITS=16 bits=16 BASE=binary */
#ifndef LIBKC3_BUF_INSPECT_U16_binary_H
#define LIBKC3_BUF_INSPECT_U16_binary_H

#include "types.h"

sw buf_inspect_u16_binary (s_buf *buf, const u16 *u);
sw buf_inspect_u16_binary_size (s_pretty *pretty, const u16 *u);

#endif /* LIBKC3_BUF_INSPECT_U16_binary_H */
