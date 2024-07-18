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
/* Gen from s.h.in BITS=16 bits=16 */
#ifndef LIBC3_S16_H
#define LIBC3_S16_H

#include "types.h"

s16 * s16_init_cast (s16 *s, const s_tag *tag);
s16 * s16_init_copy (s16 *s, const s16 *src);
s16 * s16_random (s16 *s);
u16 * s16_random_uniform (s16 *s, s16 min, s16 max);
s_tag *   s16_sqrt (const s16 x, s_tag *dest);

#endif /* LIBC3_S16_H */
