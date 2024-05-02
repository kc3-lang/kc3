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
/* Gen from u.h.in BITS=64 bits=64 */
#ifndef LIBC3_U64_H
#define LIBC3_U64_H

#include "types.h"

u64 * u64_init_cast
(u64 *u, const s_sym * const *type, const s_tag *tag);

u64 * u64_init_copy (u64 *u, const u64 *src);
u64 * u64_random (u64 *u);
u64 * u64_random_uniform (u64 *u, u64 max);
s_tag *   u64_sqrt (const u64 x, s_tag *dest);

#endif /* LIBC3_U64_H */
