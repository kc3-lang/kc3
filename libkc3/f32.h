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
#ifndef LIBKC3_F32_H
#define LIBKC3_F32_H

#include "types.h"

f32 * f32_init_cast (f32 *x, p_sym *type, const s_tag *tag);
f32 * f32_init_copy (f32 *x, f32 src);

/* Observers. */
f32 f32_clamp (f32 x, f32 min, f32 max);

/* Operators. */
f32 f32_random (void);

#endif /* LIBKC3_F32_H */
