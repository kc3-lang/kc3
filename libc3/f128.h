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
#ifndef LIBC3_F128_H
#define LIBC3_F128_H

#include "types.h"

f128 * f128_init_cast (f128 *x, const s_tag *tag);
f128 * f128_init_copy (f128 *x, const f128 *src);
f128 * f128_random (f128 *x);

#endif /* LIBC3_F128_H */
