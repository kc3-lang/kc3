/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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

s64 * s64_init_cast
(s64 *s, const s_sym * const *type, const s_tag *tag);
s64 * s64_init_copy (s64 *s, const s64 *src);
s64 * s64_random (s64 *s);
u64 * s64_random_uniform (s64 *s, s64 min, s64 max);
s_tag *   s64_sqrt (const s64 x, s_tag *dest);

#endif /* LIBKC3_S64_H */