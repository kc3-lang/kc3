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
#ifndef LIBC3_WEB_EC3_H
#define LIBC3_WEB_EC3_H

#include "types.h"

/* Stack-allocation compatible functions, call ec3_clean after use. */
void    ec3_clean (s_ec3 *ec3);
s_ec3 * ec3_init (s_ec3 *ec3);

/* Operators. */
sw     ec3_buf_parse (s_ec3 *ec3, s_buf *buf);

/* Observers. */
sw     ec3_render (const s_ec3 *ec3, s_buf *buf, s_map *map);
s_fn * ec3_to_render_fn (const s_ec3 *ec3, s_fn *dest);

#endif /* LIBC3_WEB_EC3_H */
