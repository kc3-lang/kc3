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
/* Gen from s.h.in BITS=W bits=w */
#ifndef LIBKC3_SW_H
#define LIBKC3_SW_H

#include "types.h"

/* Casts. */
sw * sw_init_1 (sw *s, const char *p);
sw * sw_init_cast
(sw *s, const s_sym * const *type, const s_tag *tag);
sw * sw_init_copy (sw *s, sw src);
sw * sw_init_str (sw *s, const s_str *str);
sw * sw_init_str_decimal (sw *s, const s_str *str);

/* Observers. */
s_tag *   sw_sqrt (const sw x, s_tag *dest);

/* Operators. */
sw * sw_random (sw *s);
uw * sw_random_uniform (sw *s, sw min, sw max);

#endif /* LIBKC3_SW_H */
