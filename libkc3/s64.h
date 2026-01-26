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
/* Gen from s.h.in BITS=64 bits=64 */
#ifndef LIBKC3_S64_H
#define LIBKC3_S64_H

#include "types.h"

/* Casts. */
s64 * s64_init_1 (s64 *s, const char *p);
s64 * s64_init_cast
(s64 *s, const s_sym * const *type, const s_tag *tag);
s64 * s64_init_copy (s64 *s, s64 src);
s64 * s64_init_str (s64 *s, const s_str *str);
s64 * s64_init_str_decimal (s64 *s, const s_str *str);

/* Observers. */
s_tag *   s64_sqrt (const s64 x, s_tag *dest);

/* Operators. */
s64 * s64_random (s64 *s);
u64 * s64_random_uniform (s64 *s, s64 min, s64 max);

#endif /* LIBKC3_S64_H */
