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
/* Gen from buf_inspect_s.h.in BITS=16 bits=16 */
#ifndef LIBC3_BUF_INSPECT_S16_H
#define LIBC3_BUF_INSPECT_S16_H

#include "types.h"

sw buf_inspect_s16 (s_buf *buf, const s16 *s);
sw buf_inspect_s16_size (const s16 *s);

#endif /* LIBC3_BUF_INSPECT_S16_H */
