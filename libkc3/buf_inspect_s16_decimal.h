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
/* Gen from buf_inspect_s_base.c.in BITS=16 bits=16 base=decimal */
#ifndef LIBKC3_BUF_INSPECT_S16_decimal_H
#define LIBKC3_BUF_INSPECT_S16_decimal_H

#include "types.h"

sw buf_inspect_s16_decimal (s_buf *buf, const s16 *s);
sw buf_inspect_s16_decimal_size (const s16 *s);

#endif /* LIBKC3_BUF_INSPECT_S16_decimal_H */