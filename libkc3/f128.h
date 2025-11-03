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
#ifndef LIBKC3_F128_H
#define LIBKC3_F128_H

#include "types.h"

#if HAVE_F128

f128 * f128_init_cast (f128 *x, const s_sym * const *type,
                       const s_tag *tag);
f128 * f128_init_copy (f128 *x, f128 src);
f128 * f128_random (f128 *x);

#endif /* HAVE_FLOAT128 */

#endif /* LIBKC3_F128_H */
