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
#ifndef LIBKC3_F64_H
#define LIBKC3_F64_H

#include "types.h"

f64 * f64_init_cast (f64 *x, p_sym *type, const s_tag *tag);
f64 * f64_init_copy (f64 *x, f64 src);
f64 * f64_random (f64 *x);

#endif /* LIBKC3_F64_H */
