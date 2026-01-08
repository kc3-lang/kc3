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
/* Gen from u.h.in BITS=W bits=w */
#ifndef LIBKC3_UW_H
#define LIBKC3_UW_H

#include "types.h"

#define uw_clean(u) ((void) u)

uw * uw_init_1 (uw *u, const char *p);
uw * uw_init_cast
(uw *u, const s_sym * const *type, const s_tag *tag);

uw * uw_init_copy (uw *u, uw src);
uw * uw_init_str (uw *u, const s_str *str);
uw * uw_init_str_hexadecimal (uw *u, const s_str *str);

uw *      uw_hash_uw (uw u, uw *dest);
uw * uw_random (uw *u);
uw * uw_random_uniform (uw *u, uw max);
s_tag *   uw_sqrt (const uw x, s_tag *dest);

#endif /* LIBKC3_UW_H */
