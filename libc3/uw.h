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
/* Gen from u.h.in BITS=W bits=w */
#ifndef LIBC3_UW_H
#define LIBC3_UW_H

#include "types.h"

uw * uw_cast (s_tag *tag, uw *dest);
uw * uw_copy (const uw *src, uw *dest);

#endif /* LIBC3_UW_H */
