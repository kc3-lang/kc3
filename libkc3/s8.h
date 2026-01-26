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
/* Gen from s.h.in BITS=8 bits=8 */
#ifndef LIBKC3_S8_H
#define LIBKC3_S8_H

#include "types.h"

/* Casts. */
s8 * s8_init_1 (s8 *s, const char *p);
s8 * s8_init_cast
(s8 *s, const s_sym * const *type, const s_tag *tag);
s8 * s8_init_copy (s8 *s, s8 src);
s8 * s8_init_str (s8 *s, const s_str *str);
s8 * s8_init_str_decimal (s8 *s, const s_str *str);

/* Observers. */
s_tag *   s8_sqrt (const s8 x, s_tag *dest);

/* Operators. */
s8 * s8_random (s8 *s);
u8 * s8_random_uniform (s8 *s, s8 min, s8 max);

#endif /* LIBKC3_S8_H */
