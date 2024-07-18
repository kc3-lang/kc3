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
/* Gen from s.h.in BITS=32 bits=32 */
#ifndef LIBC3_S32_H
#define LIBC3_S32_H

#include "types.h"

s32 * s32_init_cast (s32 *s, const s_tag *tag);
s32 * s32_init_copy (s32 *s, const s32 *src);
s32 * s32_random (s32 *s);

#endif /* LIBC3_S32_H */
