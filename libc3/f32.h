/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_F32_H
#define LIBC3_F32_H

#include "types.h"

f32 * f32_init_cast (f32 *x, const s_sym * const *type,
                     const s_tag *tag);
f32 * f32_init_copy (f32 *x, const f32 *src);
f32 * f32_random (f32 *x);

#endif /* LIBC3_F32_H */
