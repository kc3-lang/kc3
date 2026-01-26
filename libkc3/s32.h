/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_S32_H
#define LIBKC3_S32_H

#include "types.h"

/* Casts. */
s32 * s32_init_1 (s32 *s, const char *p);
s32 * s32_init_cast
(s32 *s, const s_sym * const *type, const s_tag *tag);
s32 * s32_init_copy (s32 *s, s32 src);
s32 * s32_init_str (s32 *s, const s_str *str);
s32 * s32_init_str_decimal (s32 *s, const s_str *str);

/* Observers. */
s_tag *   s32_sqrt (const s32 x, s_tag *dest);

/* Operators. */
s32 * s32_random (s32 *s);
u32 * s32_random_uniform (s32 *s, s32 min, s32 max);

#endif /* LIBKC3_S32_H */
