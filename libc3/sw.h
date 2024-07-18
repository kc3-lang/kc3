/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_SW_H
#define LIBC3_SW_H

#include "types.h"

sw * sw_init_cast (sw *s, const s_tag *tag);
sw * sw_init_copy (sw *s, const sw *src);
sw * sw_random (sw *s);
uw * sw_random_uniform (sw *s, sw min, sw max);
s_tag *   sw_sqrt (const sw x, s_tag *dest);

#endif /* LIBC3_SW_H */
