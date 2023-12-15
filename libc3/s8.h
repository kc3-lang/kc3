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
/* Gen from s.h.in BITS=8 bits=8 */
#ifndef LIBC3_S8_H
#define LIBC3_S8_H

#include "types.h"

s8 * s8_init_cast (s8 *dest, const s_tag *tag);
s8 * s8_init_copy (s8 *dest, const s8 *src);
s8 * s8_random (s8 *dest);

#endif /* LIBC3_S8_H */
