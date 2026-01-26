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
/* Gen from u.h.in BITS=8 bits=8 */
#ifndef LIBKC3_U8_H
#define LIBKC3_U8_H

#include "types.h"

#define u8_clean(u) ((void) u)

u8 * u8_init_1 (u8 *u, const char *p);
u8 * u8_init_cast
(u8 *u, const s_sym * const *type, const s_tag *tag);

u8 * u8_init_copy (u8 *u, u8 src);
u8 * u8_init_str (u8 *u, const s_str *str);
u8 * u8_init_str_hexadecimal (u8 *u, const s_str *str);

uw *      u8_hash_uw (u8 u, uw *dest);
u8 * u8_random (u8 *u);
u8 * u8_random_uniform (u8 *u, u8 max);
s_tag *   u8_sqrt (const u8 x, s_tag *dest);

#endif /* LIBKC3_U8_H */
