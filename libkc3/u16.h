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
/* Gen from u.h.in BITS=16 bits=16 */
#ifndef LIBKC3_U16_H
#define LIBKC3_U16_H

#include "types.h"

#define u16_clean(u) ((void) u)

u16 * u16_init_1 (u16 *u, const char *p);
u16 * u16_init_cast
(u16 *u, const s_sym * const *type, const s_tag *tag);

u16 * u16_init_copy (u16 *u, u16 src);
u16 * u16_init_str (u16 *u, const s_str *str);
u16 * u16_init_str_hexadecimal (u16 *u, const s_str *str);

uw *      u16_hash_uw (u16 u, uw *dest);
u16 * u16_random (u16 *u);
u16 * u16_random_uniform (u16 *u, u16 max);
s_tag *   u16_sqrt (const u16 x, s_tag *dest);

#endif /* LIBKC3_U16_H */
