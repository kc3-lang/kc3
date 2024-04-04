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
/* Gen from u.h.in BITS=32 bits=32 */
#ifndef LIBC3_U32_H
#define LIBC3_U32_H

#include "types.h"

u32 * u32_init_cast
(u32 *u, const s_sym *type, const s_tag *tag);

u32 * u32_init_copy (u32 *u, const u32 *src);
u32 * u32_random (u32 *u);
u32 * u32_random_uniform (u32 *u, u32 max);
s_tag *   u32_sqrt (const u32 x, s_tag *dest);

#endif /* LIBC3_U32_H */
