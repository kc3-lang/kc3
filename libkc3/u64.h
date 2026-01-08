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
/* Gen from u.h.in BITS=64 bits=64 */
#ifndef LIBKC3_U64_H
#define LIBKC3_U64_H

#include "types.h"

#define u64_clean(u) ((void) u)

u64 * u64_init_1 (u64 *u, const char *p);
u64 * u64_init_cast
(u64 *u, const s_sym * const *type, const s_tag *tag);

u64 * u64_init_copy (u64 *u, u64 src);
u64 * u64_init_str (u64 *u, const s_str *str);
u64 * u64_init_str_hexadecimal (u64 *u, const s_str *str);

uw *      u64_hash_uw (u64 u, uw *dest);
u64 * u64_random (u64 *u);
u64 * u64_random_uniform (u64 *u, u64 max);
s_tag *   u64_sqrt (const u64 x, s_tag *dest);

#endif /* LIBKC3_U64_H */
