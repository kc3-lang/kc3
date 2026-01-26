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
#ifndef LIBKC3_F80_H
#define LIBKC3_F80_H

#include "types.h"

#if HAVE_F80

f80 * f80_init_cast (f80 *x, const s_sym * const *type,
                     const s_tag *tag);
f80 * f80_init_copy (f80 *x, f80 src);
f80 * f80_random (f80 *x);

#endif /* HAVE_F80 */

#endif /* LIBKC3_F80_H */
