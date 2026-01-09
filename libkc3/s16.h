/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_S16_H
#define LIBKC3_S16_H

#include "types.h"

/* Casts. */
s16 * s16_init_1 (s16 *s, const char *p);
s16 * s16_init_cast
(s16 *s, const s_sym * const *type, const s_tag *tag);
s16 * s16_init_copy (s16 *s, s16 src);
s16 * s16_init_str (s16 *s, const s_str *str);
s16 * s16_init_str_decimal (s16 *s, const s_str *str);

/* Observers. */
s_tag *   s16_sqrt (const s16 x, s_tag *dest);

/* Operators. */
s16 * s16_random (s16 *s);
u16 * s16_random_uniform (s16 *s, s16 min, s16 max);

#endif /* LIBKC3_S16_H */
